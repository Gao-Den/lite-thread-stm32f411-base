/**
 ******************************************************************************
 * @author: GaoDen
 * @date:   04/12/2024
 ******************************************************************************
**/

#ifndef __NRF_MAC_H__
#define __NRF_MAC_H__

#ifdef __cplusplus
 extern "C" {
#endif

#include <stdint.h>
#include <stdio.h>
#include "xprintf.h"

#include "lt_task.h"
#include "lt_message.h"
#include "lt_timer.h"
#include "lt_fsm.h"

#include "lt_common.h"
#include "lt_log.h"
#include "lt_console.h"

#include "nrf_data.h"
#include "nrf_link.h"
#include "nrf_phy.h"
#include "nrf_sig.h"

#define NRF_MAC_HEADER_LEN              (sizeof(nrf_mac_header_t))
#define NRF_MAC_MAX_PAYLOAD_LEN         (NRF_PHY_MAX_PAYLOAD_LEN - NRF_MAC_HEADER_LEN)

typedef enum {
    NRF_MAC_STATE_IDLING,
    NRF_MAC_STATE_SENDING,
    NRF_MAC_STATE_RECEIVING,
} nrf_mac_state_t;

typedef struct {
    uint8_t mac_sequence;
    uint8_t frame_num;
    uint8_t frame_sequence;
    uint8_t frame_crc;
} nrf_mac_header_t;

typedef struct {
    nrf_mac_header_t header;
    uint8_t payload[NRF_MAC_MAX_PAYLOAD_LEN];
} __LT_PACKETED__ nrf_mac_frame_t;

extern void task_nrf_mac_handler(lt_msg_t* msg);
extern uint8_t nrf_mac_calc_frame_cs(uint8_t* data, uint16_t len);

#ifdef __cplusplus
}
#endif

#endif /* __NRF_MAC_H__ */
