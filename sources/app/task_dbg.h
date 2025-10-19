/**
 ******************************************************************************
 * @author: GaoDen
 * @date:   27/05/2025
 ******************************************************************************
**/

#ifndef __TASK_DBG_H__
#define __TASK_DBG_H__

#ifdef __cplusplus
 extern "C" {
#endif

#include <stdint.h>
#include <stdio.h>

#include "lt_message.h"

extern void task_dbg_handler(lt_msg_t* msg);

#ifdef __cplusplus
}
#endif

#endif /* __TASK_DBG_H__ */
