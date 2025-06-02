/**
 ******************************************************************************
 * @author: GaoDen
 * @date:   03/12/2024
 ******************************************************************************
**/

#ifndef __NRF_DATA_H__
#define __NRF_DATA_H__

#ifdef __cplusplus
 extern "C" {
#endif

#include "lt_task.h"
#include "lt_message.h"
#include "lt_timer.h"
#include "lt_fsm.h"

#include "lt_common.h"
#include "lt_log.h"
#include "lt_console.h"

#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>

#define NRF_PDU_MAX_POOL_SIZE               (32)
#define NRF_PDU_MAX_PAYLOAD_SIZE            (128)
#define NRF_PDU_NULL                        ((nrf_pdu_t*)0)

#define PDU_ENTRY_CRITICAL()                ENTRY_CRITICAL()
#define PDU_EXIT_CRITICAL()                 EXIT_CRITICAL()
#define PDU_FATAL(t, i)                     FATAL(t, i)

/******************************************************************************
* interface message define
*******************************************************************************/
typedef struct {
    uint8_t des_task_id;
    uint8_t type;
    uint8_t sig;
} nrf_if_msg_header_t;

/* pure message */
typedef struct {
    nrf_if_msg_header_t header;
} nrf_if_msg_pure_t;

/* common message */
typedef struct {
    nrf_if_msg_header_t header;

    uint8_t len;
    uint8_t data[LT_COMMON_MSG_DATA_SIZE];
} nrf_if_msg_common_t;

/******************************************************************************
* protocol data unit define
*******************************************************************************/
typedef enum {
    NRF_PDU_TYPE_IDLING,
    NRF_PDU_TYPE_SENDING,
    NRF_PDU_TYPE_RECEIVING,
} nrf_pdu_type_t;

typedef struct nrf_pdu_t {
    /* pdu management */
    struct nrf_pdu_t* next;

    /* pdu header */
    uint8_t id;
    uint8_t type;
    uint32_t len;

    /* pdu payload */
    uint8_t payload[NRF_PDU_MAX_PAYLOAD_SIZE];
} nrf_pdu_t;

extern void nrf_pdu_pool_init();
extern nrf_pdu_t* nrf_pdu_malloc();
extern void nrf_pdu_free(nrf_pdu_t* pdu);
extern nrf_pdu_t* nrf_pdu_get(uint8_t id);

#ifdef __cplusplus
}
#endif

#endif /* __NRF_DATA_H__ */
