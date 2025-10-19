/**
 ******************************************************************************
 * @author: GaoDen
 * @date:   03/12/2024
 ******************************************************************************
**/

#include "nrf_data.h"

#if defined (NRF_DATA_DBG_EN)
    #define NRF_DATA_LOG(fmt, ...)              LT_LOG(fmt, ##__VA_ARGS__)
#else
    #define NRF_DATA_LOG(fmt, ...)
#endif

/* nrf pdu pool */
static nrf_pdu_t nrf_pdu_pool[NRF_PDU_MAX_POOL_SIZE];
static nrf_pdu_t* free_list_pdu_pool;

/*****************************************************************************
 * pdu pool
 *****************************************************************************/
void nrf_pdu_pool_init() {
    free_list_pdu_pool = (nrf_pdu_t*)nrf_pdu_pool;

    for (uint32_t i = 0; i < NRF_PDU_MAX_POOL_SIZE; i++) {
        nrf_pdu_pool[i].id = i;
        nrf_pdu_pool[i].type = NRF_PDU_TYPE_IDLING;
        if (i == (NRF_PDU_MAX_POOL_SIZE - 1)) {
            nrf_pdu_pool[i].next = NRF_PDU_NULL;
        }
        else {
            nrf_pdu_pool[i].next = (nrf_pdu_t*)&nrf_pdu_pool[i + 1];
        }
    }
}

nrf_pdu_t* nrf_pdu_malloc() {
    nrf_pdu_t* allocated_pdu = free_list_pdu_pool;

    PDU_ENTRY_CRITICAL();
    if (allocated_pdu == NRF_PDU_NULL) {
        PDU_FATAL("PDU", 0x01);
    }
    else {
        free_list_pdu_pool = allocated_pdu->next;
    }
    PDU_EXIT_CRITICAL();

    return allocated_pdu;
}

void nrf_pdu_free(nrf_pdu_t* pdu) {
    PDU_ENTRY_CRITICAL();
    pdu->type = NRF_PDU_TYPE_IDLING;
    pdu->next = free_list_pdu_pool;
    free_list_pdu_pool = pdu;
    PDU_EXIT_CRITICAL();
}

nrf_pdu_t* nrf_pdu_get(uint8_t id) {
    if (id > NRF_PDU_MAX_POOL_SIZE) {
        PDU_FATAL("PDU", 0x02);
    }
    return (nrf_pdu_t*)&nrf_pdu_pool[id];
}
