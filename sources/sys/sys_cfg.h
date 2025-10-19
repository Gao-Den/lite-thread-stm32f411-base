/**
 ******************************************************************************
 * @author: GaoDen
 * @date:   18/11/2024
 ******************************************************************************
**/

#ifndef __SYS_CFG_H__
#define __SYS_CFG_H__

#ifdef __cplusplus
 extern "C" {
#endif 

#include <stdint.h>
#include "lt_console.h"

#define SYS_PRINT(fmt, ...)			LT_LOG((const char*)fmt, ##__VA_ARGS__)
#define CONSOLE_BAUDRATE			(115200)

typedef enum {
	SYS_IRQ_PRIO_SYSTEM_TICK = 1,
	SYS_IRQ_PRIO_USART1,
	SYS_IRQ_PRIO_NRF24,
} system_irq_prio_t;

/******************************************************************************
* system configurate functions
*******************************************************************************/
extern void sys_cfg_common();
extern void sys_cfg_clock();
extern void sys_cfg_tick();

/******************************************************************************
* system memory functions
*******************************************************************************/
extern uint32_t sys_stack_fill();
extern uint32_t sys_stack_used();
extern uint32_t sys_stack_get_size();

#ifdef __cplusplus
}
#endif

#endif /* __SYS_CFG_H__ */
