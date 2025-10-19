/**
 ******************************************************************************
 * @author: GaoDen
 * @date:   27/05/2025
 ******************************************************************************
**/

#include "task_console.h"

#include "lt_shell.h"
#include "lt_common.h"

volatile shell_t shell;

void task_console_handler(lt_msg_t* msg) {

    uint8_t first_char = *(uint8_t*)get_data_common_msg(msg);
    
    switch (msg->signal) {
    case SHELL_CMD_PARSER: {
    }
        break;

    default: {
    }
        break;
    }

    switch (cmd_line_parser((cmd_line_t*)lt_shell_table, get_data_common_msg(msg))) {
    case CMD_SUCCESS: {
    }
        break;

    case CMD_NOT_FOUND: {
        if (first_char != '\r' && first_char != '\n') {
            LT_LOG("cmd unknown\n");
            LT_LOG("try 'help' to get all commands\n\n");
        }
    }
        break;

    case CMD_TOO_LONG: {
        LT_LOG("cmd too long\n");
    }
        break;

    case CMD_TABLE_NOT_FOUND: {
        LT_LOG("cmd table not found\n");
        LT_LOG("please register cmd table before using cmd line parser\n");
    }
        break;

    default: {
        LT_LOG("cmd error\n");
    }
        break;
    }

    LT_LOG("#");
}

void task_polling_console_handler() {
    volatile uint8_t c = 0;

    while (ring_buffer_char_is_empty(&ring_buffer_console_rev) == false) {

        ENTRY_CRITICAL();
        c = ring_buffer_char_get(&ring_buffer_console_rev);
        EXIT_CRITICAL();
        
        if (shell.index < SHELL_CMD_INPUT_MAX_SIZE - 1) {

            if (c == '\r' || c == '\n') { /* linefeed */

                xputc('\r');
                xputc('\n');

                shell.data[shell.index] = c;
                shell.data[shell.index + 1] = 0;
                task_post_common_msg(TASK_CONSOLE_ID, SHELL_CMD_PARSER, (uint8_t*)&shell.data[0], shell.index + 2);

                shell.index = 0;
            }
            else {

                xputc(c);

                if (c == 8 && shell.index) { /* backspace */
                    shell.index--;
                }
                else {
                    shell.data[shell.index++] = c;
                }
            }
        }
        else {
            LT_LOG("\nerror: cmd too long, cmd size: %d, try again !\n", SHELL_CMD_INPUT_MAX_SIZE);
            shell.index = 0;
        }
    }
}
