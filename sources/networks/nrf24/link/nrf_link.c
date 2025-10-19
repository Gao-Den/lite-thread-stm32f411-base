/**
 ******************************************************************************
 * @author: GaoDen
 * @date:   04/12/2024
 ******************************************************************************
**/

#include "nrf_link.h"
#include "task_list.h"

#if defined (NRF_LINK_DBG_EN)
	#define NRF_LINK_LOG(fmt, ...)         LT_LOG(fmt, ##__VA_ARGS__)
#else
	#define NRF_LINK_LOG(fmt, ...)
#endif

fsm_t nrf_link_fsm;

/* nrf pdu fifo */
fifo_t nrf_pdu_fifo;
static uint8_t nrf_pdu_fifo_buffer[NRF_PDU_MAX_POOL_SIZE - 1];

void task_nrf_link_handler(lt_msg_t* msg) {
    fsm_dispatch(&nrf_link_fsm, msg);
} 

void nrf_link_fsm_init(lt_msg_t* msg) {
	switch (msg->signal) {
	case NRF_LINK_INIT: {
		NRF_LINK_LOG("[nrf_link] NRF_LINK_INIT\n");

		/* init pdu pool */
		nrf_pdu_pool_init();

		/* nrf pdu fifo init */
		fifo_init(&nrf_pdu_fifo, nrf_pdu_fifo_buffer, NRF_PDU_MAX_POOL_SIZE, sizeof(uint8_t));

		/* init physical layer */
		task_post_pure_msg(TASK_NRF_PHY_ID, NRF_PHY_INIT);

		/* init mac layer */
		task_post_pure_msg(TASK_NRF_MAC_ID, NRF_MAC_INIT);

		/* switch to idle state */
		FSM_TRAN(&nrf_link_fsm, nrf_link_fsm_idle);
	}
		break;

	default:
		break;
	}
}

void nrf_link_fsm_idle(lt_msg_t* msg) {
	switch (msg->signal) {
	case NRF_LINK_PURE_MSG_OUT: {
		NRF_LINK_LOG("[nrf_link] NRF_LINK_PURE_MSG_OUT\n");
		if (fifo_is_empty(&nrf_pdu_fifo)) {
			nrf_if_msg_pure_t if_pure_msg;
			if_pure_msg.header.des_task_id = msg->if_des_task_id;
			if_pure_msg.header.sig = msg->if_signal;
			if_pure_msg.header.type = msg->if_msg_type;
		
			nrf_pdu_t* pdu = nrf_pdu_malloc();
			pdu->len = NRF_LINK_HEADER_LEN + sizeof(nrf_if_msg_pure_t);
			pdu->type = NRF_PDU_TYPE_SENDING;
			
			nrf_link_t* nrf_link = (nrf_link_t*)(pdu->payload);
			nrf_link->header.type = if_pure_msg.header.type;
			nrf_link->header.payload_len = sizeof(nrf_if_msg_pure_t);
			memcpy(nrf_link->payload, (uint8_t*)&if_pure_msg, sizeof(nrf_if_msg_pure_t));
			
			/* send pdu_id to MAC layer */
			uint8_t get_pdu_id = pdu->id;
			fifo_put(&nrf_pdu_fifo, (uint8_t*)&get_pdu_id);
			task_post_pure_msg(TASK_NRF_MAC_ID, NRF_MAC_HANDLE_STATE);
		}
		else {
			task_post_pure_msg(TASK_NRF_LINK_ID, NRF_LINK_SEND_MSG_BUSY);
		}
		break;
	}

	case NRF_LINK_COMMON_MSG_OUT: {
		NRF_LINK_LOG("[nrf_link] NRF_LINK_COMMON_MSG_OUT\n");
		if (fifo_is_empty(&nrf_pdu_fifo)) {
			nrf_if_msg_common_t if_common_msg;
			if_common_msg.header.des_task_id = ((lt_common_msg_t*)msg)->msg_header.if_des_task_id;
			if_common_msg.header.sig = ((lt_common_msg_t*)msg)->msg_header.if_signal;
			if_common_msg.header.type = ((lt_common_msg_t*)msg)->msg_header.if_msg_type;

			if_common_msg.len = ((lt_common_msg_t*)msg)->data_size;
			memcpy((nrf_if_msg_common_t*)if_common_msg.data, get_data_common_msg(msg), LT_COMMON_MSG_DATA_SIZE);
			
			nrf_pdu_t* pdu = nrf_pdu_malloc();
			pdu->len = NRF_LINK_HEADER_LEN + sizeof(nrf_if_msg_common_t);
			pdu->type = NRF_PDU_TYPE_SENDING;

			nrf_link_t* nrf_link = (nrf_link_t*)(pdu->payload);
			nrf_link->header.type = if_common_msg.header.type;
			nrf_link->header.payload_len = sizeof(nrf_if_msg_common_t);
			memcpy(nrf_link->payload, (uint8_t*)&if_common_msg, sizeof(nrf_if_msg_common_t));

			/* send pdu_id to MAC layer */
			uint8_t get_pdu_id = pdu->id;
			fifo_put(&nrf_pdu_fifo, (uint8_t*)&get_pdu_id);
			task_post_pure_msg(TASK_NRF_MAC_ID, NRF_MAC_HANDLE_STATE);
		}
		else {
			task_post_pure_msg(TASK_NRF_LINK_ID, NRF_LINK_SEND_MSG_BUSY);
		}
	}
		break;

	case NRF_LINK_RECV_MSG: {
		NRF_LINK_LOG("[nrf_link] NRF_LINK_RECV_MSG\n");
		uint8_t pdu_id = *(uint8_t*)get_data_common_msg(msg);
		nrf_pdu_t* pdu_get = nrf_pdu_get(pdu_id);
		nrf_link_t* nrf_link = (nrf_link_t*)(pdu_get->payload);

		if (nrf_link->header.type == PURE_MSG_TYPE) {
			nrf_if_msg_pure_t* msg_get = (nrf_if_msg_pure_t*)(nrf_link->payload);
			task_post_pure_msg(msg_get->header.des_task_id, msg_get->header.sig);
		}
		else if (nrf_link->header.type == COMMON_MSG_TYPE) {
			nrf_if_msg_common_t* msg_get = (nrf_if_msg_common_t*)(nrf_link->payload);
			task_post_common_msg(msg_get->header.des_task_id, msg_get->header.sig, msg_get->data, msg_get->len);
		}
		
		nrf_pdu_free(pdu_get);
		NRF_LINK_LOG("[nrf_link] pdu_id free: %d\n", pdu_id);
		break;
	}

	case NRF_LINK_SEND_MSG_DONE: {
		NRF_LINK_LOG("[nrf_link] NRF_LINK_SEND_MSG_DONE\n");
		uint8_t get_pdu_id = *(uint8_t*)get_data_common_msg(msg);
		NRF_LINK_LOG("[nrf_link] pdu_id free: %d\n", get_pdu_id);
		nrf_pdu_t* pdu_get = nrf_pdu_get(get_pdu_id);
		nrf_pdu_free(pdu_get);
		break;
	}

	case NRF_LINK_SEND_MSG_ERR: {
		NRF_LINK_LOG("[nrf_link] NRF_LINK_SEND_MSG_ERR\n");
		uint8_t get_pdu_id = *(uint8_t*)get_data_common_msg(msg);
		NRF_LINK_LOG("[nrf_link] pdu_id free: %d\n", get_pdu_id);
		nrf_pdu_t* pdu_get = nrf_pdu_get(get_pdu_id);
		nrf_pdu_free(pdu_get);
	}
		break;

	case NRF_LINK_SEND_MSG_BUSY: {
		NRF_LINK_LOG("[nrf_link] NRF_LINK_SEND_MSG_BUSY\n");
	}
		break;

	default:
		break;
	}
}

void nrf_task_post_pure_msg(uint8_t if_des_task_id, uint8_t if_sig) {
	lt_msg_t* msg = get_pure_msg();
	msg->signal = NRF_LINK_PURE_MSG_OUT;
	msg->if_des_task_id = if_des_task_id;
	msg->if_signal = if_sig;
	msg->if_msg_type = PURE_MSG_TYPE;
	task_post(TASK_NRF_LINK_ID, msg);
}

void nrf_task_post_common_msg(uint8_t if_des_task_id, uint8_t if_sig, uint8_t* data, uint8_t len) {
	lt_msg_t* msg = get_common_msg();
	msg->signal = NRF_LINK_COMMON_MSG_OUT;
	msg->if_des_task_id = if_des_task_id;
	msg->if_signal = if_sig;
	msg->if_msg_type = COMMON_MSG_TYPE;
	set_data_common_msg(msg, data, len);
	task_post(TASK_NRF_LINK_ID, msg);
}
