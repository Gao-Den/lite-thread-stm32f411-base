
/**
 ******************************************************************************
 * @author: GaoDen
 * @date:   28/05/2025
 ******************************************************************************
**/

#ifndef __LT_COMMON_H__
#define __LT_COMMON_H__

#ifdef __cplusplus
 extern "C" {
#endif 

#include <stdio.h>
#include <stdint.h>

#include "sys_irq.h"
#include "lt_log.h"

#define ENTRY_CRITICAL()            disable_interrupts()
#define EXIT_CRITICAL()             enable_interrupts()

typedef void (*pf_lt_ctrl_common)();
typedef void (*pf_lt_ctrl_delay)(uint32_t);

typedef enum {
    APP_MODE_DEBUG,
    APP_MODE_RELEASE,
} app_mode_t;

/* lt common function */
extern void lt_sys_ctrl_reboot_init(pf_lt_ctrl_common pf_reboot);
extern void lt_sys_ctrl_reboot();
extern app_mode_t lt_get_app_mode();
extern void lt_set_app_mode(app_mode_t mode);

/* lt system control delay */
extern void lt_sys_ctrl_delay_ms_init(pf_lt_ctrl_delay pf_delay_ms);
extern void lt_sys_ctrl_delay_ms(uint32_t ms);
extern void lt_sys_ctrl_delay_us_init(pf_lt_ctrl_delay pf_delay_us);
extern void lt_sys_ctrl_delay_us(uint32_t us);

/* lt control led status */
extern void lt_ctrl_led_init(pf_lt_ctrl_common led_on, pf_lt_ctrl_common led_off);
extern void lt_ctrl_led_life_on();
extern void lt_ctrl_led_life_off();

#ifdef __cplusplus
}
#endif

#endif /* __LT_COMMON_H__ */
