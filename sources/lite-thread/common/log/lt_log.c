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
static lt_log_fatal_t fatal_log[LT_LOG_FATAL_OBJECT_MAX_SIZE];
static uint32_t fatal_log_default_addr;
static uint32_t fatal_log_write_index = 0;
static uint32_t fatal_log_block_used = 0;

/* fatal log flash function */
static pf_log_flash_read pf_flash_read = (pf_log_flash_read)0;
static pf_log_flash_write pf_flash_write = (pf_log_flash_write)0;
static pf_log_flash_erase pf_flash_erase = (pf_log_flash_erase)0;

void lt_log_init(pf_log_flash_read flash_read, pf_log_flash_write flash_write, pf_log_flash_erase flash_erase, uint32_t log_address) {
    memset(fatal_log, 0, sizeof(fatal_log));

    /* assign pf flash */
    pf_flash_read = flash_read;
    pf_flash_write = flash_write;
    pf_flash_erase = flash_erase;
    
    /* set fatal log address */
    fatal_log_default_addr = log_address;

    /* get fatal log object */
    pf_flash_read(fatal_log_default_addr, (uint8_t*)&fatal_log_block_used, sizeof(fatal_log_block_used));
    pf_flash_read(fatal_log_default_addr + sizeof(fatal_log_block_used), (uint8_t*)&fatal_log_write_index, sizeof(fatal_log_write_index));
    pf_flash_read(fatal_log_default_addr + sizeof(fatal_log_block_used) + sizeof(fatal_log_write_index), (uint8_t*)&fatal_log, sizeof(fatal_log));

    LT_LOG_KERNEL("[fatal_log] fatal log initialized successfully\n");
}

void lt_fatal_log(const char* type, uint8_t code) {
    if ((pf_flash_read == (pf_log_flash_read)0) || (pf_flash_write == (pf_log_flash_read)0)) {
        return;
    }

    ENTRY_CRITICAL();

    LT_LOG("\n");
    LT_LOG("-> FATAL ERROR !\n");
    LT_LOG("fatal_type: %s \tfatal_code: 0x%02X\n\n", type, code);

    /* get active object info */
    lt_msg_t current_active_object = *(lt_msg_t*)get_current_active_object();

    /* fatal log object update */
    fatal_log_write_index = fatal_log_write_index % LT_LOG_FATAL_OBJECT_MAX_SIZE;
    if (fatal_log_block_used < LT_LOG_FATAL_OBJECT_MAX_SIZE) {
        fatal_log_block_used++;
    }

    fatal_log[fatal_log_write_index].code = code;
    memset(fatal_log[fatal_log_write_index].type, 0, 8);
    strcpy((char*)fatal_log[fatal_log_write_index].type, type);
    fatal_log[fatal_log_write_index].src_task_id = current_active_object.src_task_id;
    fatal_log[fatal_log_write_index].des_task_id = current_active_object.des_task_id;
    fatal_log[fatal_log_write_index].msg_type = current_active_object.msg_type;
    fatal_log[fatal_log_write_index].signal = current_active_object.signal;
    fatal_log_write_index++;

    /* fatal log dump to flash */
    if (pf_flash_erase != (pf_log_flash_erase)0) {
        pf_flash_erase();
    }

    pf_flash_write(fatal_log_default_addr, (uint8_t*)&fatal_log_block_used, sizeof(fatal_log_block_used));
    pf_flash_write(fatal_log_default_addr + sizeof(fatal_log_block_used), (uint8_t*)&fatal_log_write_index, sizeof(fatal_log_write_index));
    pf_flash_write(fatal_log_default_addr + sizeof(fatal_log_block_used) + sizeof(fatal_log_write_index), (uint8_t*)&fatal_log, sizeof(fatal_log));

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
        lt_sys_ctrl_delay_ms(500);
        lt_sys_ctrl_reboot();
    }
}

void lt_fatal_log_dump() {
    if ((pf_flash_read == (pf_log_flash_read)0) || (pf_flash_write == (pf_log_flash_read)0)) {
        return;
    }

    uint32_t get_fatal_log_block_used;
    pf_flash_read(fatal_log_default_addr, (uint8_t*)&get_fatal_log_block_used, sizeof(get_fatal_log_block_used));
    pf_flash_read(fatal_log_default_addr + sizeof(fatal_log_block_used) + sizeof(fatal_log_write_index), (uint8_t*)&fatal_log, sizeof(fatal_log));

    LT_LOG("\n");
    LT_LOG("fatal log information\n");
    LT_LOG("fatal log trace: %d\n", get_fatal_log_block_used);
    for (uint32_t i = 0; i < get_fatal_log_block_used; i++) {
        LT_LOG("src_task_id: %d \tdes_task_id: %d \tmsg_type: %d \tsignal: %d \tfatal_code: 0x%02X \tfatal_type: %s\n",
                                                                                            fatal_log[i].src_task_id, \
                                                                                            fatal_log[i].des_task_id, \
                                                                                            fatal_log[i].msg_type, \
                                                                                            fatal_log[i].signal, \
                                                                                            fatal_log[i].code, \
                                                                                            fatal_log[i].type);
    }
    LT_LOG("\n");
}

void lt_fatal_log_reset() {
    memset((lt_log_fatal_t*)&fatal_log, 0, sizeof(fatal_log));
    if ((pf_flash_read == (pf_log_flash_read)0) || (pf_flash_write == (pf_log_flash_read)0)) {
        return;
    }
    if (pf_flash_erase != (pf_log_flash_erase)0) {
        pf_flash_erase();
    }

    /* fatal log erase */
    fatal_log_block_used = 0;
    fatal_log_write_index = 0;
    pf_flash_write(fatal_log_default_addr, (uint8_t*)&fatal_log_block_used, sizeof(fatal_log_block_used));
    pf_flash_write(fatal_log_default_addr + sizeof(fatal_log_block_used), (uint8_t*)&fatal_log_write_index, sizeof(fatal_log_write_index));
    pf_flash_write(fatal_log_default_addr + sizeof(fatal_log_block_used) + sizeof(fatal_log_write_index), (uint8_t*)&fatal_log, sizeof(fatal_log));

    LT_LOG("[fatal_log] fatal log erased successfully\n");
}
