/**
 ******************************************************************************
 * @author: GaoDen
 * @date:   03/05/2024
 ******************************************************************************
**/

#ifndef __LT_TASK_H__
#define __LT_TASK_H__

#ifdef __cplusplus
 extern "C" {
#endif 

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "lt_config.h"
#include "lt_message.h"

typedef uint8_t task_id_t;
typedef void (*pf_task)(lt_msg_t*);
typedef void (*pf_task_polling)();

typedef enum {
    TASK_PRIORITY_LEVEL_0 = 0x00,
    TASK_PRIORITY_LEVEL_1,
    TASK_PRIORITY_LEVEL_2,
    TASK_PRIORITY_LEVEL_3,
    TASK_PRIORITY_LEVEL_4,
    TASK_PRIORITY_LEVEL_5,
    TASK_PRIORITY_LEVEL_6,
    TASK_PRIORITY_LEVEL_7,
} task_priority_t;

/* kernel task */
enum {
    TASK_TIMER_TICK_ID,
    TASK_CONSOLE_ID,
    USER_DEFINE_TASK_ID,
};

enum {
    TASK_POLLING_CONSOLE_ID,
    USER_DEFINE_TASK_POLLING_ID,
};

typedef struct {
    uint8_t mask;
    task_priority_t priority;
    lt_msg_t* qhead;
    lt_msg_t* qtail;
} lt_tcb_t;

typedef struct {
    task_id_t task_id;
    task_priority_t priority;
    pf_task task_handler;
    const uint8_t* info;
} lt_task_t;

typedef struct {
    task_id_t id;
    uint8_t ability;
    pf_task_polling task_polling_handler;
    const uint8_t* info;
} lt_task_polling_t;

/* active object */
extern void task_init();
extern void task_create(lt_task_t* task);
extern void task_create_table(lt_task_t* lt_task_table);
extern void task_post(task_id_t task_id, lt_msg_t* msg);
extern void task_post_pure_msg(task_id_t des_task_id, uint8_t signal);
extern void task_post_common_msg(task_id_t des_task_id, uint8_t signal, uint8_t* data, uint8_t len);
extern void task_post_dynamic_msg(task_id_t des_task_id, uint8_t signal, uint8_t* data, uint32_t len);
extern int task_run();

/* polling object */
extern void task_polling_create(lt_task_polling_t* task_polling);
extern void task_polling_create_table(lt_task_polling_t* lt_task_table);
extern void task_polling_run();
extern void task_polling_set_ability(task_id_t task_polling_id, uint8_t ability);

/* log object */
extern void task_init_info();
extern lt_task_t* get_current_task_info();
extern lt_msg_t* get_current_active_object();

#ifdef __cplusplus
}
#endif

#endif /* __LT_TASK_H__ */
