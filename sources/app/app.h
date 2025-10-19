/**
 ******************************************************************************
 * @author: GaoDen
 * @date:   27/05/2025
 ******************************************************************************
**/

#ifndef __APP_H__
#define __APP_H__

#ifdef __cplusplus
 extern "C" {
#endif

#include <stdio.h>

#define APP_VERSION         "1.1.0"

#include "lt_message.h"

#if defined (APP_RELEASE)
    #define APP_TITLE       "lite_thread_stm32f411_release_v" APP_VERSION
#else
    #define APP_TITLE       "lite_thread_stm32f411_debug_v" APP_VERSION
#endif

/*************************************************************************/
/* APP DEFINE SIGNAL 
**************************************************************************/
enum {
    /* TASK LIFE */
    LIFE_SYSTEM_CHECK = USER_DEFINE_APP_SIGNAL,

    /* TASK DEBUG */
    DEBUG_1,
    DEBUG_2,
    DEBUG_3,

    /* END OF APP SIGNAL */
    END_OF_USER_APP_SIGNAL,
};

/*************************************************************************/
/* IF DEFINE SIGNAL 
**************************************************************************/
enum {
    REV_DBG_1,
};

/* main application */
extern int app();

/* polling system tick */
extern void sys_irq_timer_10ms();

#ifdef __cplusplus
}
#endif

#endif /* __APP_H__ */
