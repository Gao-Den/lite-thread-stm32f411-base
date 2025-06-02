/**
 ******************************************************************************
 * @author: GaoDen
 * @date:   27/05/2025
 ******************************************************************************
**/

#include "lt_log.h"

#include "lt_console.h"
#include "lt_common.h"

/* fatal log object */
static lt_log_fatal_t fatal_log[FATAL_LOG_OBJECT_MAX_SIZE];
static uint32_t fatal_log_default_addr;
static uint32_t fatal_log_write_index = 0;
static uint32_t fatal_count = 0;

/* fatal log flash function */
static pf_log_flash_read pf_flash_read = (pf_log_flash_read)0;
static pf_log_flash_write pf_flash_write = (pf_log_flash_write)0;
static pf_log_flash_write_prewrite pf_flash_prewrite = (pf_log_flash_write_prewrite)0;

void lt_log_init(pf_log_flash_read flash_read, pf_log_flash_write flash_write, pf_log_flash_write_prewrite flash_write_pre, uint32_t log_address) {
    memset(fatal_log, 0, sizeof(fatal_log));

    /* assign pf flash */
    pf_flash_read = flash_read;
    pf_flash_write = flash_write;
    pf_flash_prewrite = flash_write_pre;
    
    /* set flash fatal log address */
    fatal_log_default_addr = log_address;
}

void lt_fatal_log(const char* type, uint8_t id) {
    if ((pf_flash_read == (pf_log_flash_read)0) || (pf_flash_write == (pf_log_flash_read)0)) {
        return;
    }

    ENTRY_CRITICAL();

    LT_LOG("\n");
    LT_LOG("-> fatal error !\n");
    LT_LOG("fatal_type: %s \tfatal_code: 0x%02X\n\n", type, id);

    /* get active object info */
    lt_msg_t current_active_object = *get_current_active_object();

    /* get fatal object */
    pf_flash_read(fatal_log_default_addr, (uint8_t*)&fatal_count, sizeof(fatal_count));
    pf_flash_read(fatal_log_default_addr + sizeof(fatal_log_default_addr), (uint8_t*)&fatal_log, sizeof(fatal_log));

    /* set fatal object */
    fatal_log_write_index = fatal_count % FATAL_LOG_OBJECT_MAX_SIZE;
    memset(fatal_log[fatal_log_write_index].type, 0, 8);
    strcpy((char*)fatal_log[fatal_log_write_index].type, type);
    fatal_log[fatal_log_write_index].id = id;
    fatal_log[fatal_log_write_index].src_task_id = current_active_object.src_task_id;
    fatal_log[fatal_log_write_index].des_task_id = current_active_object.des_task_id;
    fatal_log[fatal_log_write_index].msg_type = current_active_object.msg_type;
    fatal_log[fatal_log_write_index].signal = current_active_object.signal;

    if (pf_flash_prewrite != (pf_log_flash_write_prewrite)0) {
        pf_flash_prewrite();
    }

    /* fatal log trace */
    fatal_count++;
    pf_flash_write(fatal_log_default_addr + sizeof(fatal_log_default_addr), (uint8_t*)&fatal_log, sizeof(fatal_log));
    pf_flash_write(fatal_log_default_addr, (uint8_t*)&fatal_count, sizeof(fatal_count));

    EXIT_CRITICAL();
    
    if (lt_get_app_mode() == APP_MODE_DEBUG) {
        while (1) {
            lt_ctrl_led_life_on();
            lt_sys_ctrl_delay_ms(50);
            lt_ctrl_led_life_off();
            lt_sys_ctrl_delay_ms(50); 
        }
    }
    else {
        lt_sys_ctrl_reboot();
    }
}

void lt_fatal_log_dump() {
    if ((pf_flash_read == (pf_log_flash_read)0) || (pf_flash_write == (pf_log_flash_read)0)) {
        return;
    }

    uint32_t get_fatal_count;
    pf_flash_read(fatal_log_default_addr, (uint8_t*)&get_fatal_count, sizeof(get_fatal_count));
    pf_flash_read(fatal_log_default_addr + sizeof(fatal_log_default_addr), (uint8_t*)&fatal_log, sizeof(fatal_log));

    LT_LOG("\n");
    LT_LOG("[fatal log trace]\n");
    LT_LOG("fatal_log_obj: %d\n", get_fatal_count);
    for (uint32_t i = 0; i < get_fatal_count; i++) {
        LT_LOG("fatal_type: %s \tfatal_code: 0x%02X \tsrc_task_id: %d \t\tdes_task_id: %d \t\tmsg_type: %d \t\tsignal: %d\n", 
                                                                                            fatal_log[i].type,
                                                                                            fatal_log[i].id,
                                                                                            fatal_log[i].src_task_id,
                                                                                            fatal_log[i].des_task_id,
                                                                                            fatal_log[i].msg_type,
                                                                                            fatal_log[i].signal);
    }

    LT_LOG("\n");
}

void lt_fatal_log_reset() {
    memset((lt_log_fatal_t*)&fatal_log, 0, sizeof(fatal_log));
    if ((pf_flash_read == (pf_log_flash_read)0) || (pf_flash_write == (pf_log_flash_read)0)) {
        return;
    }
    if (pf_flash_prewrite != (pf_log_flash_write_prewrite)0) {
        pf_flash_prewrite();
    }

    /* fatal log erase */
    fatal_count = 0;
    pf_flash_write(fatal_log_default_addr, (uint8_t*)&fatal_count, sizeof(fatal_count));
    pf_flash_write(fatal_log_default_addr + sizeof(fatal_log_default_addr), (uint8_t*)&fatal_log, sizeof(fatal_log));
}
