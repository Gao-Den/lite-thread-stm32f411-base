/**
 ******************************************************************************
 * @author: GaoDen
 * @date:   03/05/2024
 ******************************************************************************
**/

#ifndef __LT_TIMER_H__
#define __LT_TIMER_H__

#ifdef __cplusplus
 extern "C" {
#endif 

#include <stdio.h>
#include <stdint.h>

#include "lt_task.h"
#include "lt_message.h"
#include "lt_config.h"

#define TIMER_ERROR					(0x00)
#define TIMER_OK					(0x01)
#define TIMER_TICK_SIG				(0x02)

#define LT_TIMER_NULL				((lt_timer_t*)0)

typedef uint8_t timer_sig_t;

typedef enum {
	TIMER_ONE_SHOT,
	TIMER_PERIODIC,
} timer_type_t;

typedef struct lt_timer_t {
	struct lt_timer_t* next;     /* manage timer message */

	task_id_t des_task_id;       /* destination task id */
	timer_sig_t signal;             /* signal for application */

	uint32_t counter;            /* decrease each timer system tick */
	uint32_t period;             /* case one-shot timer, this field is equa 0 */
} lt_timer_t;

extern void timer_init();
extern void timer_tick(uint32_t tick);
extern void lt_task_timer_handler(lt_msg_t* msg);

extern uint8_t timer_set(task_id_t des_task_id, timer_sig_t signal, uint32_t duty, timer_type_t timer_type);
extern uint8_t timer_remove(task_id_t des_task_id, timer_sig_t signal);
extern uint8_t get_timer_pool_free();

#ifdef __cplusplus
}
#endif

#endif /* __LT_TIMER_H__ */
