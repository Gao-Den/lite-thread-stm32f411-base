/**
 ******************************************************************************
 * @author: GaoDen
 * @date:   04/12/2024
 ******************************************************************************
**/

#ifndef __NRF_PHY_H__
#define __NRF_PHY_H__

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

#include "nrf_data.h"
#include "nrf_link.h"
#include "nrf_sig.h"

#define NRF_PHY_MAX_PAYLOAD_LEN             (32)
#define PHY_STATE_HARDWARE_NONE             (0)
#define PHY_STATE_HARDWARE_STARTED          (1)
#define NRF_PHY_CHANEL_CFG                  (120)

#define NRF_PHY_IF_DIRECTLY                 (0xFD)
#define NRF_PHY_IF_SOF_DIRECTLY             (0xFE)

extern uint8_t device_mac_addr[5];

extern void task_nrf_phy_handler(lt_msg_t* msg);
extern void nrf_phy_irq();
extern void nrf_phy_switch_ptx_mode();
extern void nrf_phy_switch_prx_mode();
extern void nrf_phy_send_directly(uint8_t* pbuf, uint8_t len);

#ifdef __cplusplus
}
#endif

#endif /* __NRF_PHY_H__ */
