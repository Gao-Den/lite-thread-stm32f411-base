/**
 ******************************************************************************
 * @author: GaoDen
 * @date:   27/05/2025
 ******************************************************************************
**/

#include "lt_shell.h"
#include "lt_config.h"
#include "task_console.h"
#include "lt_common.h"

/* shell management */
cmd_line_t lt_shell_table[SHELL_TABLE_MAX_SIZE];
static uint16_t shell_table_size = 0;

/* shell buffer */
uint8_t buffer_console_rev[SHELL_RING_BUFFER_REV_MAX_SIZE];
ring_buffer_char_t ring_buffer_console_rev;

/* shell initial */
int32_t shell_help(uint8_t* argv);

void lt_shell_init() {
    /* ring buffer init */
    ring_buffer_char_init(&ring_buffer_console_rev, buffer_console_rev, SHELL_RING_BUFFER_REV_MAX_SIZE);

    /* clear the shell table */
    for (uint8_t i = 0; i < SHELL_TABLE_MAX_SIZE; i++) {
        lt_shell_table[i].cmd = (const int8_t*)0;
        lt_shell_table[i].func_exe = (pf_cmd_func)0;
        lt_shell_table[i].info = (const uint8_t*)0;
    }

    /* shell inital */
    cmd_line_t shell_help_cmd;
    shell_help_cmd.cmd = (const int8_t*)"help";
    shell_help_cmd.func_exe = shell_help;
    shell_help_cmd.info = (const uint8_t*)"show all commands";
    shell_help_cmd.sub_help = (const uint8_t*)0;
    lt_shell_register(&shell_help_cmd);
}

void lt_shell_register(cmd_line_t* shell) {
    if (shell_table_size < SHELL_TABLE_MAX_SIZE) {
        /* register command */
        lt_shell_table[shell_table_size].cmd = shell->cmd;
        lt_shell_table[shell_table_size].func_exe = shell->func_exe;
        lt_shell_table[shell_table_size].info = shell->info;
        lt_shell_table[shell_table_size].sub_help = shell->sub_help;
        shell_table_size++;

        /* increment the shell table size */
        lt_shell_table[shell_table_size].cmd = (const int8_t*)0;
        lt_shell_table[shell_table_size].func_exe = (pf_cmd_func)0;
        lt_shell_table[shell_table_size].info = (const uint8_t*)0;
        lt_shell_table[shell_table_size].sub_help = (const uint8_t*)0;
    }
    else {
        /* TODO: implement shell over size */
    }
}

void lt_shell_register_table(cmd_line_t* shell_table) {
    uint16_t size = 0;
    while (shell_table[size].cmd != (const int8_t*)0) {
        size++;
    }

    if ((shell_table_size + size) <= SHELL_TABLE_MAX_SIZE) {
        for (uint16_t i = 0; i < size; i++) {
            lt_shell_register(&shell_table[i]);
        }
    }
    else {
        /* TODO: implement shell over size */
    }
}

void lt_shell_getc(volatile uint8_t c) {
    ENTRY_CRITICAL();
    ring_buffer_char_put(&ring_buffer_console_rev, c);
    EXIT_CRITICAL();
}

/* shell register initial */
int32_t shell_help(uint8_t* argv) {
    uint32_t index = 0;
    
    LT_LOG("\n\033[1mCOMMAND LIST:\033[0m\n\n");
    
    while(lt_shell_table[index].cmd != (const int8_t*)0) {
        /* command name in bold */
        LT_LOG("\033[1m%-12s\033[0m -> %s\n", lt_shell_table[index].cmd, lt_shell_table[index].info);
        
        /* usage in italic */
        if (lt_shell_table[index].sub_help != (const uint8_t*)0) {
            LT_LOG("\033[3m%s\033[0m\n\n", lt_shell_table[index].sub_help);
        }
        else {
            LT_LOG("\n");
        }
        
        index++;
    }
    
    LT_LOG("\n");
    return 0;
}
