/**
 ******************************************************************************
 * @author: GaoDen
 * @date:   18/11/2024
 ******************************************************************************
**/

#include "task_dbg.h"

#include "lt_task.h"
#include "lt_message.h"
#include "lt_timer.h"

#include "io_cfg.h"
#include "sys_cfg.h"
#include "sys_ctrl.h"

#include "led.h"
#include "utils.h"

#include "app.h"
#include "app_dbg.h"
#include "task_list.h"

void task_dbg_handler(lt_msg_t* msg) {
    switch (msg->signal) {
    case DEBUG_1: {
        APP_PRINT("[task_debug] DEBUG_1\n");
    }
        break;

    default: {
        
    }
        break;
    }
}
