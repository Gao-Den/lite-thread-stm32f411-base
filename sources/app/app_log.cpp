/**
 ******************************************************************************
 * @author: GaoDen
 * @date:   19/10/2025
 ******************************************************************************
**/

#include "app_log.h"

#include "app_dbg.h"
#include "app_flash.h"
#include "flash.h"

/* app log object */
static app_log_t app_log[APP_LOG_MAX_ITEM_SIZE];
static uint32_t app_log_write_index = 0;
static uint32_t app_log_block_used = 0;

const char* app_log_table[] = {
    "SENSOR_READING_ERR",
    "NETWORK_RECONNECT",
    "NETWORK_DISCONNECTED",
    "MQTT_RECONNECT",
    "MQTT_DISCONNECTED",
};

void app_log_init() {
    memset(app_log, 0, sizeof(app_log));

    /* get fatal log object */
    flash_read(APP_FLASH_APP_LOG_OBJ_ADDR, (uint8_t*)&app_log_block_used, sizeof(app_log_block_used));
    flash_read(APP_FLASH_APP_LOG_OBJ_ADDR + sizeof(app_log_block_used), (uint8_t*)&app_log_write_index, sizeof(app_log_write_index));
    flash_read(APP_FLASH_APP_LOG_OBJ_ADDR + sizeof(app_log_block_used) + sizeof(app_log_write_index), (uint8_t*)&app_log, sizeof(app_log));

    LT_LOG("[app_log] app log initialized successfully\n");
}

void app_log_dbg(app_log_type_t type, app_log_info_t info) {
    ENTRY_CRITICAL();

    /* get active object info */
    lt_msg_t current_active_object = *(lt_msg_t*)get_current_active_object();

    /* app log object update */
    app_log_write_index = app_log_write_index % APP_LOG_MAX_ITEM_SIZE;

    if (app_log_block_used < APP_LOG_MAX_ITEM_SIZE) {
        app_log_block_used++;
    }

    APP_PRINT("[app_log] app_log_write_index: %d\n", app_log_write_index);

    app_log[app_log_write_index].type = type;
    app_log[app_log_write_index].info = info;
    app_log[app_log_write_index].src_task_id = current_active_object.src_task_id;
    app_log[app_log_write_index].des_task_id = current_active_object.des_task_id;
    app_log[app_log_write_index].msg_type = current_active_object.msg_type;
    app_log[app_log_write_index].signal = current_active_object.signal;
    app_log_write_index++;

    flash_erase_sector(APP_FLASH_APP_LOG_OBJ_ADDR);
    flash_write(APP_FLASH_APP_LOG_OBJ_ADDR, (uint8_t*)&app_log_block_used, sizeof(app_log_block_used));
    flash_write(APP_FLASH_APP_LOG_OBJ_ADDR + sizeof(app_log_block_used), (uint8_t*)&app_log_write_index, sizeof(app_log_write_index));
    flash_write(APP_FLASH_APP_LOG_OBJ_ADDR + sizeof(app_log_block_used) + sizeof(app_log_write_index), (uint8_t*)&app_log, sizeof(app_log));

    EXIT_CRITICAL();
}

void app_log_dump() {
    uint32_t app_log_counter;
    flash_read(APP_FLASH_APP_LOG_OBJ_ADDR, (uint8_t*)&app_log_counter, sizeof(app_log_counter));
    flash_read(APP_FLASH_APP_LOG_OBJ_ADDR + sizeof(app_log_block_used) + sizeof(app_log_write_index), (uint8_t*)&app_log, sizeof(app_log));

    LT_LOG("app log information:\n");
    LT_LOG("app log trace: %d\n", app_log_counter);

    for (uint32_t i = 0; i < app_log_counter; i++) {
        if (app_log[i].type == APP_LOG_TYPE_INFO) {
            LT_LOG("[INF] src_task_id: %d \tdes_task_id: %d \tmsg_type: %d \tsignal: %d \tlog_info: %s\n",
                                                                                                app_log[i].src_task_id, \
                                                                                                app_log[i].des_task_id, \
                                                                                                app_log[i].msg_type, \
                                                                                                app_log[i].signal, \
                                                                                                (const char*)app_log_table[app_log[i].info]);
        }
    }

    for (uint32_t i = 0; i < app_log_counter; i++) {
        if (app_log[i].type == APP_LOG_TYPE_WARN) {
            LT_LOG("[WARN] src_task_id: %d \tdes_task_id: %d \tmsg_type: %d \tsignal: %d \tlog_info: %s\n",
                                                                                                app_log[i].src_task_id, \
                                                                                                app_log[i].des_task_id, \
                                                                                                app_log[i].msg_type, \
                                                                                                app_log[i].signal, \
                                                                                                (const char*)app_log_table[app_log[i].info]);
        }
    }

    for (uint32_t i = 0; i < app_log_counter; i++) {
        if (app_log[i].type == APP_LOG_TYPE_ERROR) {
            LT_LOG("[ERROR] src_task_id: %d \tdes_task_id: %d \tmsg_type: %d \tsignal: %d \tlog_info: %s\n",
                                                                                                app_log[i].src_task_id, \
                                                                                                app_log[i].des_task_id, \
                                                                                                app_log[i].msg_type, \
                                                                                                app_log[i].signal, \
                                                                                                (const char*)app_log_table[app_log[i].info]);
        }
    }
}

void app_log_erase() {
    memset((app_log_t*)&app_log, 0, sizeof(app_log));

    /* fatal log erase */
    app_log_block_used = 0;
    app_log_write_index = 0;
    flash_erase_sector(APP_FLASH_APP_LOG_OBJ_ADDR);
    flash_write(APP_FLASH_APP_LOG_OBJ_ADDR, (uint8_t*)&app_log_block_used, sizeof(app_log_block_used));
    flash_write(APP_FLASH_APP_LOG_OBJ_ADDR + sizeof(app_log_block_used), (uint8_t*)&app_log_write_index, sizeof(app_log_write_index));
    flash_write(APP_FLASH_APP_LOG_OBJ_ADDR + sizeof(app_log_block_used) + sizeof(app_log_write_index), (uint8_t*)&app_log, sizeof(app_log));

    LT_LOG("[app_log] app log erased successfully\n");
}
