/**
 ******************************************************************************
 * @author: GaoDen
 * @date:   04/12/2024
 ******************************************************************************
**/

#include "nrf_phy.h"

#include "hal_nrf_hw.h"
#include "hal_nrf.h"

#include "app.h"
#include "app_dbg.h"
#include "task_list.h"

#if defined (NRF_PHY_DBG_EN)
	#define NRF_PHY_LOG(fmt, ...)         LT_LOG(fmt, ##__VA_ARGS__)
#else
	#define NRF_PHY_LOG(fmt, ...)
#endif

static uint8_t nrf_phy_frame_buffer[NRF_PHY_MAX_PAYLOAD_LEN];
static uint8_t phy_state = PHY_STATE_HARDWARE_NONE;

/* mac address */
uint8_t device_mac_addr[5] = {0x4E, 0x48, 0x41, 0x4E, 0x54};
static uint8_t* nrf_get_des_phy_addr();
static uint8_t* nrf_get_src_phy_addr();

bool nrf_phy_irq_state = false;

static void nrf_phy_hal_mask(uint8_t* pbuf);

void task_nrf_phy_handler(lt_msg_t* msg) {
	switch (msg->signal) {
	case NRF_PHY_INIT: {
        NRF_PHY_LOG("[nrf_phy] NRF_PHY_INIT\n");

		CE_LOW();
		sys_ctrl_delay_ms(100);

		hal_nrf_set_power_mode(HAL_NRF_PWR_DOWN);
		hal_nrf_get_clear_irq_flags();

		hal_nrf_close_pipe(HAL_NRF_ALL); /* first close all radio pipes, Pipe 0 and 1 open by default */
		hal_nrf_open_pipe(HAL_NRF_PIPE0, true); /* open pipe0, without/autoack (autoack) */

		hal_nrf_set_crc_mode(HAL_NRF_CRC_16BIT); /* operates in 16bits CRC mode */
		hal_nrf_set_auto_retr(5, 750); /* enable auto retransmit */

		hal_nrf_set_address_width(HAL_NRF_AW_5BYTES); /* 5 bytes address width */

		APP_DBG("MAC initial\n");
		APP_DBG("MAC device address: 0x%02X:0x%02X:0x%02X:0x%02X:0x%02X\n", 
														device_mac_addr[0], 
														device_mac_addr[1], 
														device_mac_addr[2], 
														device_mac_addr[3], 
														device_mac_addr[4]);

		hal_nrf_set_address(HAL_NRF_TX, (uint8_t*)nrf_get_src_phy_addr()); /* set device's addresses */
		hal_nrf_set_address(HAL_NRF_PIPE0, (uint8_t*)nrf_get_src_phy_addr()); /* set recieving address on pipe0 */

		hal_nrf_set_operation_mode(HAL_NRF_PTX);
		hal_nrf_set_rx_pload_width((uint8_t)HAL_NRF_PIPE0, NRF_PHY_MAX_PAYLOAD_LEN);

		hal_nrf_set_rf_channel(NRF_PHY_CHANEL_CFG);
		hal_nrf_set_output_power(HAL_NRF_0DBM);
		hal_nrf_set_lna_gain(HAL_NRF_LNA_HCURR);
		hal_nrf_set_datarate(HAL_NRF_2MBPS);

		hal_nrf_set_power_mode(HAL_NRF_PWR_UP); /* power up device */

		hal_nrf_set_irq_mode(HAL_NRF_MAX_RT, true);
		hal_nrf_set_irq_mode(HAL_NRF_TX_DS, true);
		hal_nrf_set_irq_mode(HAL_NRF_RX_DR, true);

		hal_nrf_flush_rx();
		hal_nrf_flush_tx();

		sys_ctrl_delay_ms(2);
		CE_HIGH();

		ENTRY_CRITICAL();
		phy_state = PHY_STATE_HARDWARE_STARTED;
		EXIT_CRITICAL(); 
	}
		break;

	case NRF_PHY_IRQ_TX_MAX_RT: {
		NRF_PHY_LOG("[nrf_phy] send error: max retries reached\n");
		task_post_pure_msg(TASK_NRF_MAC_ID, NRF_MAC_SEND_FRAME_ERR);
	}
		break;

	case NRF_PHY_IRQ_TX_DS: {
		NRF_PHY_LOG("[nrf_phy] packet sent done !\n");
		if (nrf_phy_irq_state) {
			nrf_phy_irq_state = false;
		}
		else {
			task_post_pure_msg(TASK_NRF_MAC_ID, NRF_MAC_SEND_FRAME_DONE);
		}
	}
		break;

	case NRF_PHY_IRQ_RX_DR: {
		NRF_PHY_LOG("[nrf_phy] packet received !\n");
		nrf_phy_hal_mask(get_data_common_msg(msg));
	}
		break;

	case NRF_PHY_IRQ_ACK_PR: {
		NRF_PHY_LOG("[nrf_phy] ack payload recieved !\n");
	}
		break;

	case NRF_PHY_IRQ_CLEAR_REQ: {
		NRF_PHY_LOG("[nrf_phy] clear irq req !\n");
	}
		break;

	case NRF_PHY_SEND_FRAME_REQ: {
		NRF_PHY_LOG("[nrf_phy] send frame req\n");
		uint8_t* payload = get_data_common_msg(msg);
		hal_nrf_write_tx_pload(payload, NRF_PHY_MAX_PAYLOAD_LEN);
	}

	default: {
    }
		break;
	}
}

void nrf_phy_irq() {
	if (phy_state == PHY_STATE_HARDWARE_NONE) {
		NRF_PHY_LOG("[nrf_phy] PHY_STATE_HARDWARE_NONE\n");
		return;
	}

	uint8_t nrf_phy_irq_mask = hal_nrf_get_clear_irq_flags();

	switch (nrf_phy_irq_mask) {
	case (1 << HAL_NRF_MAX_RT): { /* max retries reached */
		hal_nrf_flush_tx(); /* flush tx fifo, avoid fifo jam */
		task_post_pure_msg(TASK_NRF_PHY_ID, NRF_PHY_IRQ_TX_MAX_RT);
	}
		break;

	case (1 << HAL_NRF_TX_DS): { /* packet sent */
		task_post_pure_msg(TASK_NRF_PHY_ID, NRF_PHY_IRQ_TX_DS);
	}
		break;

	case (1 << HAL_NRF_RX_DR): /* packet received */
		if (!hal_nrf_rx_fifo_empty()) {
			uint8_t payload_len;
			payload_len = hal_nrf_read_rx_pload(nrf_phy_frame_buffer);
			if (payload_len == NRF_PHY_MAX_PAYLOAD_LEN) {
				task_post_common_msg(TASK_NRF_PHY_ID, NRF_PHY_IRQ_RX_DR, nrf_phy_frame_buffer, NRF_PHY_MAX_PAYLOAD_LEN);
			}
			else {
				FATAL("PHY", 0x01);
			}
		}
		break;

	case ((1 << HAL_NRF_RX_DR) | ( 1 << HAL_NRF_TX_DS)): { /* ack payload recieved */
		if (!hal_nrf_rx_fifo_empty()) {
			hal_nrf_read_rx_pload(nrf_phy_frame_buffer);
			task_post_common_msg(TASK_NRF_PHY_ID, NRF_PHY_IRQ_ACK_PR, nrf_phy_frame_buffer, NRF_PHY_MAX_PAYLOAD_LEN);
		}
	}
		break;

	default: {
		task_post_pure_msg(TASK_NRF_PHY_ID, NRF_PHY_IRQ_CLEAR_REQ);
	}
		break;
	}
}

void nrf_phy_switch_prx_mode() {
	NRF_PHY_LOG("[nrf_phy] switch to rx mode !\n");
	CE_LOW();
	hal_nrf_set_address(HAL_NRF_PIPE0, (uint8_t*)nrf_get_des_phy_addr()); /* sets recieving address on pipe0 */
	hal_nrf_set_operation_mode(HAL_NRF_PRX);
	sys_ctrl_delay_us(150);
	CE_HIGH();
}

void nrf_phy_switch_ptx_mode() {
	NRF_PHY_LOG("[nrf_phy] switch to tx mode !\n");
	CE_LOW();
	sys_ctrl_delay_ms(100);
	hal_nrf_set_address(HAL_NRF_TX, (uint8_t*)nrf_get_src_phy_addr()); /* set device's addresses */
	hal_nrf_set_operation_mode(HAL_NRF_PTX);
	hal_nrf_flush_rx();
	hal_nrf_flush_tx();
	sys_ctrl_delay_ms(2);
	CE_HIGH();
}

uint8_t* nrf_get_des_phy_addr() {
	NRF_PHY_LOG("[nrf_data] nrf_get_des_phy_addr(%02X %02X %02X %02X %02X)\n", \
			device_mac_addr[0],
			device_mac_addr[1],
			device_mac_addr[2],
			device_mac_addr[3],
			device_mac_addr[4]);
	return device_mac_addr;
}

uint8_t* nrf_get_src_phy_addr() {
	NRF_PHY_LOG("[nrf_data] nrf_get_src_phy_addr(%02X %02X %02X %02X %02X)\n", \
			device_mac_addr[0],
			device_mac_addr[1],
			device_mac_addr[2],
			device_mac_addr[3],
			device_mac_addr[4]);
	return device_mac_addr;
}

void nrf_phy_hal_mask(uint8_t* pbuf) {
	if (pbuf[0] == NRF_PHY_IF_SOF_DIRECTLY) {
		task_post_common_msg(TASK_IF_ID, NRF_PHY_IF_DIRECTLY, pbuf, NRF_PHY_MAX_PAYLOAD_LEN);
	}
	else {
		task_post_common_msg(TASK_NRF_MAC_ID, NRF_MAC_RECV_FRAME, pbuf, NRF_PHY_MAX_PAYLOAD_LEN);
	}
}

void nrf_phy_send_directly(uint8_t* pbuf, uint8_t len) {
	memcpy(nrf_phy_frame_buffer, pbuf, len);
	nrf_phy_irq_state = true;
	hal_nrf_flush_tx();
	hal_nrf_write_tx_pload(nrf_phy_frame_buffer, NRF_PHY_MAX_PAYLOAD_LEN);
}
