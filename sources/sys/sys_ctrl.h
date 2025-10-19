/**
 ******************************************************************************
 * @author: GaoDen
 * @date:   20/11/2024
 ******************************************************************************
**/

#ifndef __SYS_CTRL_H__
#define __SYS_CTRL_H__

#ifdef __cplusplus
 extern "C" {
#endif 

#include <stdint.h>

/******************************************************************************
* system utility functions
*******************************************************************************/
extern uint32_t sys_ctrl_millis();
extern void sys_ctrl_delay_us(uint32_t us);
extern void sys_ctrl_delay_ms(uint32_t ms);
extern void sys_ctrl_independent_watchdog_init();
extern void sys_ctrl_independent_watchdog_reset();
extern void sys_ctrl_reset();

#ifdef __cplusplus
}
#endif

#endif /* __SYS_CTRL_H__ */
