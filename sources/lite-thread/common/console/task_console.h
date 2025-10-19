/**
 ******************************************************************************
 * @author: GaoDen
 * @date:   27/05/2025
 ******************************************************************************
**/

#ifndef __TASK_CONSOLE_H__
#define __TASK_CONSOLE_H__

#ifdef __cplusplus
 extern "C" {
#endif 

#include <stdint.h>
#include "lt_task.h"

#include "lt_console.h"
#include "lt_shell.h"

typedef struct {
	uint8_t index;
	uint8_t data[SHELL_CMD_INPUT_MAX_SIZE];
} shell_t;

extern void task_polling_console_handler();
extern void task_console_handler(lt_msg_t* msg);

#ifdef __cplusplus
}
#endif

#endif /* __TASK_CONSOLE_H__ */
