/**
 ******************************************************************************
 * @author: GaoDen
 * @date:   27/05/2025
 ******************************************************************************
**/

#ifndef __TASK_LIFE_H__
#define __TASK_LIFE_H__

#ifdef __cplusplus
 extern "C" {
#endif 

#include <stdint.h>
#include "lt_task.h"
#include "led.h"

extern led_t led_life;
extern void task_life_handler(lt_msg_t* msg);

#ifdef __cplusplus
}
#endif

#endif /* __TASK_LIFE_H__ */
