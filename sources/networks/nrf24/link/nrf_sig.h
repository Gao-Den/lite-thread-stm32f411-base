/**
 ******************************************************************************
 * @author: GaoDen
 * @date:   03/12/2024
 ******************************************************************************
**/

#ifndef __NRF_SIG_H__
#define __NRF_SIG_H__

#ifdef __cplusplus
 extern "C" {
#endif

#include <stdint.h>
#include <stdio.h>

#include "app.h"

/******************************************************************************
* nrf link signal define
*******************************************************************************/
enum {
    NRF_LINK_INIT = END_OF_USER_APP_SIGNAL,
    NRF_LINK_PURE_MSG_OUT,
    NRF_LINK_COMMON_MSG_OUT,
    NRF_LINK_RECV_MSG,
	NRF_LINK_SEND_MSG_DONE,
	NRF_LINK_SEND_MSG_ERR,
	NRF_LINK_SEND_MSG_BUSY,

    NRF_MAC_INIT = 0,
	NRF_MAC_HANDLE_STATE,
	NRF_MAC_HANDLE_MSG_OUT,
	NRF_MAC_HANDLE_MSG_IN,
	NRF_MAC_SEND_FRAME,
	NRF_MAC_SEND_FRAME_DONE,
	NRF_MAC_SEND_FRAME_ERR,
	NRF_MAC_RECV_FRAME,

	NRF_PHY_INIT,
	NRF_PHY_IRQ_TX_MAX_RT,
	NRF_PHY_IRQ_TX_DS,
	NRF_PHY_IRQ_RX_DR,
	NRF_PHY_IRQ_ACK_PR,
	NRF_PHY_SEND_FRAME_REQ,
	NRF_PHY_IRQ_CLEAR_REQ,
	NRF_PHY_REV_MODE_REQ,
	NRF_PHY_SEND_MODE_REQ,
};

#ifdef __cplusplus
}
#endif

#endif /* __NRF_SIG_H__ */
