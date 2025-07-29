/**
 ******************************************************************************
 * @author: GaoDen
 * @date:   03/05/2024
 ******************************************************************************
**/

#ifndef __LT_CONFIG_H__
#define __LT_CONFIG_H__

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>

/*****************************************************************************
 * DEFINITION: lite thread version
 *
 *****************************************************************************/
#define LITE_THREAD_KERNEL_VERSION          "1.4"

#define LT_DISABLE                          (0x00)
#define LT_ENABLE                           (0x01)

/*****************************************************************************
 * DEFINITION: tasking
 *
 *****************************************************************************/
#define TASK_ACTIVE_OBJECT_MAX_SIZE         (16)
#define TASK_POLLING_MAX_SIZE               (8)
#define TASK_PRIORITY_MAX_SIZE              (8)

/*****************************************************************************
 * DEFINITION: message
 *
 *****************************************************************************/
/* size of message pool */
#define LT_PURE_MSG_POOL_SIZE               (8)
#define LT_COMMON_MSG_POOL_SIZE             (8)
#define LT_COMMON_MSG_DATA_SIZE             (64)
#define LT_DYNAMIC_MSG_POOL_SIZE            (8)

/*****************************************************************************
 * DEFINITION: timer
 *
 *****************************************************************************/
#define LT_TIMER_POOL_SIZE                  (16)

/*****************************************************************************
 * DEFINITION: console
 *
 *****************************************************************************/
#define SHELL_TABLE_MAX_SIZE                (16)
#define SHELL_RING_BUFFER_REV_MAX_SIZE      (256)
#define SHELL_CMD_INPUT_MAX_SIZE            (32)

/*****************************************************************************
 * DEFINITION: log
 *
 *****************************************************************************/
#define FATAL_LOG_OBJECT_MAX_SIZE           (32)

/*****************************************************************************
 * DEFINITION: attribute
 *
 *****************************************************************************/
#define LT_PACKETED                         __attribute__((__packed__))
#define LT_WEAK                             __attribute__((__weak__))

#ifdef __cplusplus
}
#endif

#endif /* __LT_CONFIG_H__ */
