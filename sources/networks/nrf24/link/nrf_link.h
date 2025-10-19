/**
 ******************************************************************************
 * @author: GaoDen
 * @date:   04/12/2024
 ******************************************************************************
**/

#ifndef __NRF_LINK_H__
#define __NRF_LINK_H__

#ifdef __cplusplus
 extern "C" {
#endif

#include <stdint.h>
#include <stdio.h>

#include "lt_task.h"
#include "lt_message.h"
#include "lt_timer.h"
#include "lt_fsm.h"

#include "lt_common.h"
#include "lt_log.h"
#include "lt_console.h"
#include "fifo.h"

#include "nrf_data.h"
#include "nrf_sig.h"

#define NRF_LINK_HEADER_LEN             (sizeof(nrf_link_header_t))
#define NRF_LINK_PAYLOAD_LEN            (NRF_PDU_MAX_PAYLOAD_SIZE - NRF_LINK_HEADER_LEN)

typedef struct {
    uint8_t type;
    uint16_t payload_len;
} __LT_PACKETED__ nrf_link_header_t;

typedef struct {
    nrf_link_header_t header;
    uint8_t payload[NRF_LINK_PAYLOAD_LEN];
} __LT_PACKETED__ nrf_link_t;

extern fsm_t nrf_link_fsm;
extern fifo_t nrf_pdu_fifo;

extern void task_nrf_link_handler(lt_msg_t* msg);
extern void nrf_link_fsm_init(lt_msg_t* msg);
extern void nrf_link_fsm_idle(lt_msg_t* msg);

extern void nrf_task_post_pure_msg(uint8_t if_des_task_id, uint8_t if_sig);
extern void nrf_task_post_common_msg(uint8_t if_des_task_id, uint8_t if_sig, uint8_t* data, uint8_t len);

#ifdef __cplusplus
}
#endif

#endif /* __NRF_LINK_H__ */
