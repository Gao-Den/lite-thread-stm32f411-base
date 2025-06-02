/**
 ******************************************************************************
 * @author: GaoDen
 * @date:   28/05/2025
 ******************************************************************************
**/

#include "lt_common.h"

/* lt common pfunction */
pf_lt_ctrl_common pf_ctrl_sys_reboot = (pf_lt_ctrl_common)0;
pf_lt_ctrl_delay pf_ctrl_delay_ms = (pf_lt_ctrl_delay)0;
pf_lt_ctrl_delay pf_ctrl_delay_us = (pf_lt_ctrl_delay)0;
pf_lt_ctrl_common pf_ctrl_led_life_on = (pf_lt_ctrl_common)0;
pf_lt_ctrl_common pf_ctrl_led_life_off = (pf_lt_ctrl_common)0;

/* app run mode */
static app_mode_t app_mode = APP_MODE_DEBUG;

void lt_sys_ctrl_reboot_init(pf_lt_ctrl_common pf_reboot) {
    pf_ctrl_sys_reboot = pf_reboot;
}

void lt_sys_ctrl_reboot() {
    if (pf_ctrl_sys_reboot) {
        pf_ctrl_sys_reboot();
    }
}

void lt_sys_ctrl_delay_ms_init(pf_lt_ctrl_delay pf_delay_ms) {
    pf_ctrl_delay_ms = pf_delay_ms;
}

void lt_sys_ctrl_delay_ms(uint32_t ms) {
    if (pf_ctrl_delay_ms) {
        pf_ctrl_delay_ms(ms);
    }
}

void lt_sys_ctrl_delay_us_init(pf_lt_ctrl_delay pf_delay_us) {
    pf_ctrl_delay_us = pf_delay_us;
}

void lt_sys_ctrl_delay_us(uint32_t us) {
    if (pf_ctrl_delay_us) {
        pf_ctrl_delay_us(us);
    }
}

void lt_ctrl_led_init(pf_lt_ctrl_common led_on, pf_lt_ctrl_common led_off) {
    pf_ctrl_led_life_on = led_on;
    pf_ctrl_led_life_off = led_off;
}

void lt_ctrl_led_life_on() {
    if (pf_ctrl_led_life_on) {
        pf_ctrl_led_life_on();
    }
}

void lt_ctrl_led_life_off() {
    if (pf_ctrl_led_life_off) {
        pf_ctrl_led_life_off();
    }
}

app_mode_t lt_get_app_mode() {
    return app_mode;
}

void lt_set_app_mode(app_mode_t mode) {
    app_mode = mode;
}
