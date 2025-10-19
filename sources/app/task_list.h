/**
 ******************************************************************************
 * @author: GaoDen
 * @date:   27/05/2025
 ******************************************************************************
**/

#ifndef __TASK_LIST_H__
#define __TASK_LIST_H__

#ifdef __cplusplus
 extern "C" {
#endif 

#include <stdint.h>
#include "lt_task.h"

#include "task_life.h"
#include "task_dbg.h"

/*************************************************************************/
/* ACTIVE OBJECT 
**************************************************************************/
enum {
    /* APP TASKS */
    TASK_DBG_ID = USER_DEFINE_TASK_ID,
    TASK_LIFE_ID,
    TASK_IF_ID,

    /* NRF24 NETWORK */
    TASK_NRF_LINK_ID,
    TASK_NRF_MAC_ID,
    TASK_NRF_PHY_ID,
    
    /* END OF TABLE */
    TASK_EOT_ID,
};

/*************************************************************************/
/* POLLING OBJECT 
**************************************************************************/
enum {
    /* POLLING TASKS */
    /* TODO: add new task polling */

    /* END OF TABLE */
    TASK_POLLING_EOT_ID,
};

extern lt_task_t app_task_table[];
extern lt_task_polling_t app_lt_task_polling_table[];

#ifdef __cplusplus
}
#endif

#endif /* __TASK_LIST_H__ */
