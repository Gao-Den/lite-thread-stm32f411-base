/**
 ******************************************************************************
 * @author: GaoDen
 * @date:   04/12/2024
 ******************************************************************************
**/

#include "nrf_mac.h"
#include "task_list.h"

#if defined (NRF_MAC_DBG_EN)
    #define NRF_MAC_LOG(fmt, ...)               LT_LOG(fmt, ##__VA_ARGS__)
#else
    #define NRF_MAC_LOG(fmt, ...)
#endif

#define SENDING_MAC_FRAME_RETRY_COUNTER_MAX		(3)
#define SENDING_MAC_PDU_RETRY_COUNTER_MAX		(3)
#define SENDING_MAC_FRAME_RETRY_INTERVAL		(15)
#define SENDING_MAC_PDU_RETRY_INTERVAL          (40)

/* mac retry transmit */
static uint8_t mac_sending_frame_retry_counter = 0;
static uint8_t mac_sending_pdu_retry_counter = 0;

/* mac tranfers */
static nrf_pdu_t* pdu_sending_frame;
static nrf_pdu_t* pdu_receiving_frame;
static nrf_mac_frame_t mac_sending_frame;
static nrf_mac_frame_t mac_receiving_frame;
static uint8_t mac_sending_sequence;
static uint8_t mac_receiving_sequence;

/* mac state */
static nrf_mac_state_t mac_state;
static nrf_mac_state_t get_mac_state();
static void set_mac_state(nrf_mac_state_t state);

void task_nrf_mac_handler(lt_msg_t* msg) {
	switch (msg->signal) {
	case NRF_MAC_INIT: {
        NRF_MAC_LOG("[nrf_mac] NRF_MAC_INIT\n");
        
        /* init mac sequence */
		mac_sending_sequence = (uint8_t)rand();
		mac_receiving_sequence = (uint8_t)rand();

        /* set mac init status */
        set_mac_state(NRF_MAC_STATE_IDLING);
	}
		break;

	case NRF_MAC_HANDLE_STATE: {
        NRF_MAC_LOG("[nrf_mac] NRF_MAC_HANDLE_STATE\n");
        if (fifo_availble(&nrf_pdu_fifo)) {
            if (get_mac_state() == NRF_MAC_STATE_IDLING) {
                /* get new pdu from fifo */
                uint8_t pdu_id;
                fifo_get(&nrf_pdu_fifo, (uint8_t*)&pdu_id);
                nrf_pdu_t* pdu_get = nrf_pdu_get(pdu_id);

                /* handling pdu */
                if (pdu_get->type == NRF_PDU_TYPE_SENDING) {
                    set_mac_state(NRF_MAC_STATE_SENDING);
                    task_post_common_msg(TASK_NRF_MAC_ID, NRF_MAC_HANDLE_MSG_OUT, (uint8_t*)&pdu_id, sizeof(pdu_id));
                }
                else if (pdu_get->type == NRF_PDU_TYPE_RECEIVING) {
                    set_mac_state(NRF_MAC_STATE_RECEIVING);
                    task_post_common_msg(TASK_NRF_LINK_ID, NRF_LINK_RECV_MSG, (uint8_t*)&pdu_id, sizeof(pdu_id));
                    set_mac_state(NRF_MAC_STATE_IDLING);
                }
            }
        }
        else {
            NRF_MAC_LOG("[nrf_mac] fifo empty !\n");
        }
	}
		break;

    case NRF_MAC_HANDLE_MSG_OUT: {
        NRF_MAC_LOG("[nrf_mac] NRF_MAC_HANDLE_MSG_OUT\n");
        /* increse mac sequence */
        mac_sending_sequence++;

        /* get pdu from pdu pool */
        uint8_t pdu_id = *(uint8_t*)get_data_common_msg(msg);
        pdu_sending_frame = nrf_pdu_get(pdu_id);
        mac_sending_frame.header.mac_sequence = mac_sending_sequence;
        mac_sending_frame.header.frame_sequence = 0;
        mac_sending_frame.header.frame_num = (pdu_sending_frame->len / NRF_MAC_MAX_PAYLOAD_LEN) + ((pdu_sending_frame->len % NRF_MAC_MAX_PAYLOAD_LEN) > 0);
        task_post_pure_msg(TASK_NRF_MAC_ID, NRF_MAC_SEND_FRAME);
    }
        break;

    case NRF_MAC_SEND_FRAME: {
        NRF_MAC_LOG("[nrf_mac] NRF_MAC_SEND_FRAME\n");
        if (get_mac_state() == NRF_MAC_STATE_SENDING) {
            /* div pdu and post mac frame to physical link */
            memcpy(mac_sending_frame.payload, (pdu_sending_frame->payload + (mac_sending_frame.header.frame_sequence * NRF_MAC_MAX_PAYLOAD_LEN)), NRF_MAC_MAX_PAYLOAD_LEN);
            mac_sending_frame.header.frame_crc = nrf_mac_calc_frame_cs(mac_sending_frame.payload, NRF_MAC_MAX_PAYLOAD_LEN);
            
            /* send frame to physical layer */
            task_post_common_msg(TASK_NRF_PHY_ID, NRF_PHY_SEND_FRAME_REQ, (uint8_t*)&mac_sending_frame, sizeof(nrf_mac_frame_t));
        }
    }
        break;

    case NRF_MAC_SEND_FRAME_DONE: {
        NRF_MAC_LOG("[nrf_mac] NRF_MAC_SEND_FRAME_DONE\n");
        if (get_mac_state() == NRF_MAC_STATE_SENDING) {
            /* reset mac_frame retry counter */
            mac_sending_frame_retry_counter = 0;

            /* last frame - send complete */
            if (mac_sending_frame.header.frame_sequence == (mac_sending_frame.header.frame_num - 1)) {
                /* notice send frame done to link layer */
                uint8_t pdu_id = pdu_sending_frame->id;
                task_post_common_msg(TASK_NRF_LINK_ID, NRF_LINK_SEND_MSG_DONE, (uint8_t*)&pdu_id, sizeof(pdu_id));
                
                /* set state is idling to handle new pdu */
                set_mac_state(NRF_MAC_STATE_IDLING);

                /* get next pdu */
                task_post_pure_msg(TASK_NRF_MAC_ID, NRF_MAC_HANDLE_STATE);
            }
            /* send next frame */
            else {
                mac_sending_frame.header.frame_sequence++;
                task_post_pure_msg(TASK_NRF_MAC_ID, NRF_MAC_SEND_FRAME);
            }
        }
    }
        break;

    case NRF_MAC_SEND_FRAME_ERR: {
        NRF_MAC_LOG("[nrf_mac] NRF_MAC_SEND_FRAME_ERR\n");
        if (get_mac_state() == NRF_MAC_STATE_SENDING) {
            if (mac_sending_frame_retry_counter < SENDING_MAC_FRAME_RETRY_COUNTER_MAX) {
                /* retry send mac frame */
                timer_set(TASK_NRF_MAC_ID, NRF_MAC_SEND_FRAME, SENDING_MAC_FRAME_RETRY_INTERVAL, TIMER_ONE_SHOT);
                mac_sending_frame_retry_counter++;
            }
            else {
                mac_sending_frame_retry_counter = 0;

                /* retry sending pdu when frame sending error > SENDING_MAC_FRAME_RETRY_COUNTER_MAX */
                if (mac_sending_pdu_retry_counter < SENDING_MAC_PDU_RETRY_COUNTER_MAX) {
                    mac_sending_sequence++;
                    mac_sending_frame.header.frame_sequence = 0;
                    mac_sending_frame.header.mac_sequence = mac_sending_sequence;
                    timer_set(TASK_NRF_MAC_ID, NRF_MAC_SEND_FRAME, SENDING_MAC_PDU_RETRY_INTERVAL, TIMER_ONE_SHOT);
                    mac_sending_pdu_retry_counter++;
                }
                else {
                    mac_sending_pdu_retry_counter = 0;

                    uint8_t pdu_id = pdu_sending_frame->id;
                    task_post_common_msg(TASK_NRF_LINK_ID, NRF_LINK_SEND_MSG_ERR, (uint8_t*)&pdu_id, sizeof(pdu_id));

                    /* get next pdu */
                    set_mac_state(NRF_MAC_STATE_IDLING);
                    task_post_pure_msg(TASK_NRF_MAC_ID, NRF_MAC_HANDLE_STATE);
                }
            }
        }
    }
        break;

    case NRF_MAC_RECV_FRAME: {
        NRF_MAC_LOG("[nrf_mac] NRF_MAC_RECV_FRAME\n");
        nrf_mac_frame_t* recv_mac_frame = (nrf_mac_frame_t*)get_data_common_msg(msg);
        uint8_t frame_crc = nrf_mac_calc_frame_cs(recv_mac_frame->payload, NRF_MAC_MAX_PAYLOAD_LEN);

        if (frame_crc == recv_mac_frame->header.frame_crc) {
            /* receive the first frame */
            if (recv_mac_frame->header.mac_sequence != mac_receiving_sequence) {
                if (recv_mac_frame->header.frame_sequence == 0) {
                    pdu_receiving_frame = nrf_pdu_malloc();
                    pdu_receiving_frame->type = NRF_PDU_TYPE_RECEIVING;
                    mac_receiving_sequence = recv_mac_frame->header.mac_sequence;
                    set_mac_state(NRF_MAC_STATE_RECEIVING);
                }
            }

            if (get_mac_state() == NRF_MAC_STATE_RECEIVING) {
                if (recv_mac_frame->header.frame_sequence < recv_mac_frame->header.frame_num) {
                    /* TODO: implement mac timeout */

                    if (recv_mac_frame->header.frame_sequence == (recv_mac_frame->header.frame_num - 1)) {
                        set_mac_state(NRF_MAC_STATE_IDLING);
                    }

                    /* send frame received */
                    memcpy((nrf_mac_frame_t*)&mac_receiving_frame, recv_mac_frame, sizeof(nrf_mac_frame_t));
                    task_post_common_msg(TASK_NRF_MAC_ID, NRF_MAC_HANDLE_MSG_IN, (uint8_t*)&mac_receiving_frame, sizeof(nrf_mac_frame_t));
                }
            }
        }
    }
        break;

    case NRF_MAC_HANDLE_MSG_IN: {
        NRF_MAC_LOG("[nrf_mac] NRF_MAC_HANDLE_MSG_IN\n");
        nrf_mac_frame_t* recv_mac_frame = (nrf_mac_frame_t*)get_data_common_msg(msg);
        memcpy(pdu_receiving_frame->payload + (recv_mac_frame->header.frame_sequence * NRF_MAC_MAX_PAYLOAD_LEN), recv_mac_frame->payload, NRF_MAC_MAX_PAYLOAD_LEN);

        if (recv_mac_frame->header.frame_sequence == (recv_mac_frame->header.frame_num - 1)) {
            uint8_t pdu_id = pdu_receiving_frame->id;
            fifo_put(&nrf_pdu_fifo, (uint8_t*)&pdu_id);
            set_mac_state(NRF_MAC_STATE_IDLING);
            task_post_pure_msg(TASK_NRF_MAC_ID, NRF_MAC_HANDLE_STATE);
        }
    }
        break;

	default:
		break;
	}
}

nrf_mac_state_t get_mac_state() {
    return mac_state;
}

void set_mac_state(nrf_mac_state_t state) {
    mac_state = state;
    if (mac_state != NRF_MAC_STATE_SENDING) {
        nrf_phy_switch_prx_mode();
    }
    else {
        nrf_phy_switch_ptx_mode();
    }
}

uint8_t nrf_mac_calc_frame_cs(uint8_t* data, uint16_t len) {
	uint8_t ret = 0;
	for (uint16_t i = 0; i < len; i++) {
		ret ^= *(data + i);
	}
	return ret;
}
