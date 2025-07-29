/**
 ******************************************************************************
 * @author: GaoDen
 * @date:   27/05/2025
 ******************************************************************************
**/

#include "shell.h"

#include "app_dbg.h"
#include "sys_ctrl.h"

#include "lt_log.h"
#include "lt_common.h"

#include "task_list.h"

/* app shell command */
int32_t shell_reboot(uint8_t* argv);
int32_t shell_dbg(uint8_t* argv);
int32_t shell_sys(uint8_t* argv);
int32_t shell_clear(uint8_t* argv);
int32_t shell_fatal(uint8_t* argv);

cmd_line_t app_shell_table[] = {
    /***********************************************************************************************************************************************************************************/
    /* DEBUG COMMAND */
    /***********************************************************************************************************************************************************************************/
    {(const int8_t*)"reboot",       shell_reboot,       (const uint8_t*)"system reboot",            (const uint8_t*)0},
    {(const int8_t*)"sys",          shell_sys,          (const uint8_t*)"shell system",             (const uint8_t*)0},
    {(const int8_t*)"clear",        shell_clear,        (const uint8_t*)"clear screen",             (const uint8_t*)0},
    {(const int8_t*)"fatal",        shell_fatal,        (const uint8_t*)"fatal log",                (const uint8_t*)"try 'fatal l' for get fatal log - 'fatal r' for reset fatal log"},
    {(const int8_t*)"dbg",          shell_dbg,          (const uint8_t*)"shell debug",              (const uint8_t*)0},
    
    /***********************************************************************************************************************************************************************************/
    /* END OF TABLE */
    /***********************************************************************************************************************************************************************************/
    {(const int8_t*)0,              (pf_cmd_func)0,     (const uint8_t*)0,                          (const uint8_t*)0}
};

int32_t shell_reboot(uint8_t* argv) {
    (void)argv;
    sys_ctrl_reset();
    return 0;
}

int32_t shell_sys(uint8_t* argv) {
    switch (*(argv + 4)) {
    case 't': {
        task_init_info();
    }
        break;

    case 'v': {
        APP_PRINT("kernel_version: %s\n", LITE_THREAD_KERNEL_VERSION);
        APP_PRINT("app_version: %s\n", APP_VERSION);
    }
        break;

    case 'm': {
        APP_PRINT("mills: %d\n", sys_ctrl_millis());
    }
        break;

    default: {
        APP_PRINT("[shell_system] unknown option !\n");
    }
        break;
    }
    
    return 0;
}

int32_t shell_clear(uint8_t* argv) {
	(void)argv;
	APP_PRINT("\033[2J\r");
	return 0;
}

int32_t shell_fatal(uint8_t* argv) {
    switch (*(argv + 6)) {
    case '1': {
        FATAL("DBG", 0xEE);
    }
        break;

    case 'l': {
        lt_fatal_log_dump();
    }
        break;

    case 'r': {
        lt_fatal_log_reset();
    }
        break;

    default: {
        APP_PRINT("[shell_fatal] unknown option !\n");
    }
        break;
    }
    
    return 0;
}

int32_t shell_dbg(uint8_t* argv) {
    switch (*(argv + 4)) {
    case '1': {
        task_post_pure_msg(TASK_DBG_ID, DEBUG_1);
    }
        break;

    default: {
        APP_PRINT("[shell_dbg] unknown option !\n");
    }
        break;
    }
    
    return 0;
}
2