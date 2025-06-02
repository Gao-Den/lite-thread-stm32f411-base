/**
 ******************************************************************************
 * @author: GaoDen
 * @date:   27/05/2025
 ******************************************************************************
**/

#include "task_life.h"

#include "lt_task.h"
#include "lt_message.h"
#include "lt_timer.h"

#include "sys_ctrl.h"

#include "app.h"
#include "app_dbg.h"
#include "task_list.h"

led_t led_life;

void task_life_handler(lt_msg_t* msg) {
    switch (msg->signal) {
    case LIFE_SYSTEM_CHECK: {
        sys_ctrl_independent_watchdog_reset();
        led_toggle(&led_life);
    }
        break;

    default:
        break;
    }
}
