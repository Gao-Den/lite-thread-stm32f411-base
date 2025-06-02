/**
 ******************************************************************************
 * @author: GaoDen
 * @date:   27/05/2025
 ******************************************************************************
**/

#include "app.h"

/* kernel include */
#include "lt_common.h"
#include "lt_task.h"
#include "lt_message.h"
#include "lt_timer.h"
#include "lt_shell.h"
#include "lt_console.h"

/* system include */
#include "io_cfg.h"
#include "lt_common.h"
#include "sys_cfg.h"
#include "sys_irq.h"
#include "sys_ctrl.h"

/* driver include */
#include "ring_buffer.h"
#include "led.h"
#include "flash.h"

/* app include */
#include "app_dbg.h"
#include "app_flash.h"
#include "shell.h"
#include "task_list.h"

/* kernel initial */
static void lt_common_init();

/* app initial */
static void app_start_timer();
static void app_task_init();

/* app flash */
void static flash_erase_log_fatal_sector();
void static flash_write_log_fatal(uint32_t address, uint8_t* pbuf, uint32_t len);
void static flash_read_log_fatal(uint32_t address, uint8_t* pbuf, uint32_t len);

int app() {
    /******************************************************************************
    * init kernel
    *******************************************************************************/
    lt_common_init();

    /* active object init */
    msg_init();
    timer_init();
    task_init();

    /* task create */
    task_create_table((lt_task_t*)&app_lt_task_table);

    /* independent watchdog init */
    sys_ctrl_independent_watchdog_init();

    /******************************************************************************
    * app task initial
    *******************************************************************************/
    app_task_init();

    /******************************************************************************
    * start timer for application
    *******************************************************************************/
    app_start_timer();

    /******************************************************************************
    * kernel start
    *******************************************************************************/
    return task_run();
}

/*****************************************************************************/
/* kernel initial function 
 */
/*****************************************************************************/
void lt_common_init() {
    /* lt common init */
    lt_sys_ctrl_delay_ms_init(sys_ctrl_delay_ms);
    lt_sys_ctrl_delay_us_init(sys_ctrl_delay_us);
    lt_sys_ctrl_reboot_init(sys_ctrl_reset);

    /* shell command init */
    lt_shell_init();
    lt_shell_register_table((cmd_line_t*)&app_shell_table);

    /* flash log init */
    lt_log_init(flash_read_log_fatal, flash_write_log_fatal, flash_erase_log_fatal_sector, APP_FLASH_LT_LOG_OBJ_ADDR);

    /* led indicator init */
    lt_ctrl_led_init(led_life_on, led_life_off);
}

/*****************************************************************************/
/* app initial function 
 */
/*****************************************************************************/

/* send the first message to tasks */
void app_task_init() {
#if defined (APP_RELEASE)
    APP_PRINT("[app_mode]: RELEASE\n");
    lt_set_app_mode(APP_MODE_RELEASE);
#else
    APP_DBG("[app_mode]: DEBUG\n");
    lt_set_app_mode(APP_MODE_DEBUG);
#endif
    APP_PRINT("[app_title]: %s\n", APP_TITLE);
    APP_PRINT("[app_version]: %s\n\n", APP_VERSION);
}

/* start software timer for application */
void app_start_timer() {
    timer_set(TASK_LIFE_ID, LIFE_SYSTEM_CHECK, 1000, TIMER_PERIODIC);
}

/*****************************************************************************/
/* app common function
 */
/*****************************************************************************/

/* hardware timer interrupt 10ms
 * used for led, button polling
 */
void sys_irq_timer_10ms() {
    led_polling(&led_life);
}

/* flash log
 * log for fatal error
 * this log will be stored in flash memory
 */
void flash_erase_log_fatal_sector() {
    flash_erase_sector(APP_FLASH_LT_LOG_OBJ_ADDR);
}

void flash_write_log_fatal(uint32_t address, uint8_t* pbuf, uint32_t len) {
    flash_write(address, pbuf, len);
}

void flash_read_log_fatal(uint32_t address, uint8_t* pbuf, uint32_t len) {
    flash_read(address, pbuf, len);
}
