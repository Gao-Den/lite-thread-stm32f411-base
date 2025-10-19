/**
 ******************************************************************************
 * @author: GaoDen
 * @date:   19/10/2025
 ******************************************************************************
**/

#ifndef __APP_LOG_H__
#define __APP_LOG_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h> 
#include <stdint.h>

#include "lt_task.h"
#include "lt_common.h"

#define APP_LOG_MAX_ITEM_SIZE               (64)

/*************************************************************************/
/* APP LOG MESSAGE DEFINE (for example)
**************************************************************************/
typedef enum {
    LOG_SENSOR_READING_ERR,

    LOG_NETWORK_RECONNECT,
    LOG_NETWORK_DISCONNECTED,

    LOG_MQTT_RECONNECT,
    LOG_MQTT_DISCONNECTED
} app_log_info_t;

/*************************************************************************/
/* APP LOG OBJECT
**************************************************************************/
typedef enum {
    APP_LOG_TYPE_INFO,
    APP_LOG_TYPE_WARN,
    APP_LOG_TYPE_ERROR,
} app_log_type_t;

typedef struct {
    app_log_type_t type;
    app_log_info_t info;
    
    /* task info */
    task_id_t src_task_id;
    task_id_t des_task_id;

    /* message info */
    uint8_t msg_type;
    uint8_t signal;
} app_log_t;

extern void app_log_init();
extern void app_log_dbg(app_log_type_t type, app_log_info_t info);
extern void app_log_dump();
extern void app_log_erase();

#ifdef __cplusplus
}
#endif

#endif /* __APP_LOG_H__ */
