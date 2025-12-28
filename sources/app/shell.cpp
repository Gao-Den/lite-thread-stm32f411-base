/**
 ******************************************************************************
 * @author: GaoDen
 * @date:   27/05/2025
 ******************************************************************************
**/

#include "shell.h"

#include "app_dbg.h"
#include "app_log.h"
#include "sys_ctrl.h"

#include "lt_log.h"
#include "lt_common.h"

#include "task_list.h"

/* common shell utilities */
#define STR_LIST_MAX_SIZE               (8)
#define STR_BUFFER_SIZE                 (128)
static char cmd_buffer[STR_BUFFER_SIZE];
static char* str_list[STR_LIST_MAX_SIZE];
static uint8_t str_list_len;
static uint8_t str_parser(char* str);
static char* str_parser_get_attr(uint8_t);

/* system shell command */
int32_t shell_reboot(uint8_t* argv);
int32_t shell_dbg(uint8_t* argv);
int32_t shell_sys(uint8_t* argv);
int32_t shell_clear(uint8_t* argv);
int32_t shell_fatal(uint8_t* argv);

/* app shell command */
int32_t shell_log(uint8_t* argv);

cmd_line_t app_shell_table[] = {
    /***********************************************************************************************************************************************************************************/
    /* SYSTEM COMMAND */
    /***********************************************************************************************************************************************************************************/
    {(const int8_t*)"reboot",       shell_reboot,       (const uint8_t*)"system reboot",            (const uint8_t*)0},
    {(const int8_t*)"sys",          shell_sys,          (const uint8_t*)"shell system",             (const uint8_t*)0},
    {(const int8_t*)"clear",        shell_clear,        (const uint8_t*)"clear screen",             (const uint8_t*)0},
    {(const int8_t*)"fatal",        shell_fatal,        (const uint8_t*)"fatal log",                (const uint8_t*)"try 'fatal l' for get fatal log, 'fatal r' for reset fatal log"},

    /***********************************************************************************************************************************************************************************/
    /* APP COMMAND */
    /***********************************************************************************************************************************************************************************/
    {(const int8_t*)"dbg",          shell_dbg,          (const uint8_t*)"shell debug",              (const uint8_t*)0},
    {(const int8_t*)"log",          shell_log,          (const uint8_t*)"app log",                  (const uint8_t*)"try 'log l' for get app log, 'log r' for reset app log"},

    /***********************************************************************************************************************************************************************************/
    /* END OF TABLE */
    /***********************************************************************************************************************************************************************************/
    {(const int8_t*)0,              (pf_cmd_func)0,     (const uint8_t*)0,                          (const uint8_t*)0}
};

/******************************************************************************
* app common shell utilities
*******************************************************************************/
uint8_t str_parser(char* str) {
    strcpy(cmd_buffer, str);
    str_list_len = 0;

    uint8_t i = 0;
    uint8_t str_list_index = 0;
    uint8_t flag_insert_str = 1;

    while (cmd_buffer[i] != 0 && cmd_buffer[i] != '\n' && cmd_buffer[i] != '\r') {
        if (cmd_buffer[i] == ' ') {
            cmd_buffer[i] = 0;
            flag_insert_str = 1;
        }
        else if (flag_insert_str) {
            str_list[str_list_index++] = &cmd_buffer[i];
            flag_insert_str = 0;
        }
        i++;
    }

    cmd_buffer[i] = 0;

    str_list_len = str_list_index;
    return str_list_len;
}

char* str_parser_get_attr(uint8_t index) {
    if (index < str_list_len) {
        return str_list[index];
    }
    return NULL;
}

/******************************************************************************
* app common shell functions
*******************************************************************************/
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
    case 'l': {
        lt_fatal_log_dump();
    }
        break;

    case 'r': {
        lt_fatal_log_reset();
    }
        break;

    case 't': {
        FATAL("DBG", 0xEE);
    }
        break;

    case '1': {
        FATAL("TEST", 0x10);
    }
        break;

    default: {
        APP_PRINT("[shell_fatal] unknown option !\n");
    }
        break;
    }
    
    return 0;
}

int32_t shell_log(uint8_t* argv) {
    switch (*(argv + 4)) {
    case 'l': {
        app_log_dump();
    }
        break;

    case 'r': {
        app_log_erase();
    }
        break;

    case '1': {
        APP_LOG_WARN(LOG_MQTT_RECONNECT);
    }
        break;

    default: {
        APP_PRINT("[shell_log] unknown option !\n");
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
