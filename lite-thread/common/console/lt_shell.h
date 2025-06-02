/**
 ******************************************************************************
 * @author: GaoDen
 * @date:   27/05/2025
 ******************************************************************************
**/

#ifndef __LT_SHELL_H__
#define __LT_SHELL_H__

#ifdef __cplusplus
 extern "C" {
#endif 

#include <stdint.h>

#include "lt_config.h"
#include "cmd_line.h"
#include "ring_buffer.h"

/* table contain commands */
extern cmd_line_t lt_shell_table[SHELL_TABLE_MAX_SIZE];
extern uint8_t buffer_console_rev[SHELL_RING_BUFFER_REV_MAX_SIZE];
extern ring_buffer_char_t ring_buffer_console_rev;

extern void lt_shell_init();
extern void lt_shell_register(cmd_line_t* shell);
extern void lt_shell_register_table(cmd_line_t* shell_table);
extern void lt_shell_getc(volatile uint8_t c);

#ifdef __cplusplus
}
#endif

#endif /* __LT_SHELL_H__ */
