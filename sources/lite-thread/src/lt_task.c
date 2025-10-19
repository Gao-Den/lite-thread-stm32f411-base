/**
 ******************************************************************************
 * @author: GaoDen
 * @date:   03/05/2024
 ******************************************************************************
**/

#include "lt_task.h"

#include "lt_log.h"
#include "lt_common.h"
#include "lt_timer.h"

#include "lt_console.h"
#include "task_console.h"

#define LOG2LKUP(val)           ((uint_fast8_t)(32U - __builtin_clz(val)))

/* active object management */
static lt_task_t task_list[TASK_ACTIVE_OBJECT_MAX_SIZE];
static lt_tcb_t active_object_task_queue[TASK_PRIORITY_MAX_SIZE];
static uint8_t lt_task_table_size = 0;
static uint8_t task_ready = 0;

/* polling object management */
static lt_task_polling_t task_polling_list[TASK_POLLING_MAX_SIZE];
static uint8_t task_polling_size = 0;

/* log object */
static lt_msg_t current_active_object;
static lt_task_t current_task_info;

/* kernel task */
lt_task_t task_console;
lt_task_polling_t task_polling_console;

/* banner */
static void lt_banner();

void lt_banner() {
    LT_LOG("\n\n\n");
    LT_LOG(" __    __  ____  ____    ____  _  _  ____  ____   __   ____\n");
    LT_LOG("(  )  (  )(_  _)(  __)  (_  _)/ )( \\(  _ \\(  __) / _\\ (    \\\n");
    LT_LOG("/ (_/\\ )(   )(   ) _)     )(  ) __ ( )   / ) _) /    \\ ) D (\n");
    LT_LOG("\\____/(__) (__) (____)   (__) \\_)(_/(__\\_)(____)\\_/\\_/(____/\n");
    LT_LOG("\n");
    LT_LOG("Kernel version: %s\n", LITE_THREAD_KERNEL_VERSION);
    LT_LOG("Author: %s\n", "GaoDen");
    LT_LOG("Build date: %s %s\n", __DATE__, __TIME__);
}

void task_init() {
    lt_tcb_t* tcb;
    uint8_t pri;
    task_polling_size = 0;

    /* init active object queue */
    for (pri = 1; pri <= TASK_PRIORITY_MAX_SIZE; pri++) {
        tcb = &active_object_task_queue[pri - 1];
        tcb->mask = (1 << (pri - 1));
        tcb->qhead = LT_MSG_NULL;
        tcb->qtail = LT_MSG_NULL;
    }

    /* console init */
    task_console.task_id = TASK_CONSOLE_ID;
    task_console.priority = TASK_PRIORITY_LEVEL_2;
    task_console.task_handler = task_console_handler;
    task_console.info = (const uint8_t*)"task console";
    task_create(&task_console);
    
    task_polling_console.id = TASK_POLLING_CONSOLE_ID;
    task_polling_console.ability = LT_ENABLE;
    task_polling_console.task_polling_handler = task_polling_console_handler;
    task_polling_console.info = (const uint8_t*)"task polling console";
    task_polling_create(&task_polling_console);

    LT_LOG_KERNEL("[task] kernel task initialized successfully\n");
}

void lt_init() {
    /* kernel banner */
    lt_banner();

    /* message init */
    msg_init();

    /* timer service init */
    timer_init();

    /* kernel task init */
    task_init();

    LT_LOG("\n");
    LT_LOG("[KERNEL] system init successfully\n");
    LT_LOG("[KERNEL] application start\n");
    LT_LOG("\n");
}

void task_create(lt_task_t* task) {
    if (lt_task_table_size > TASK_ACTIVE_OBJECT_MAX_SIZE) {
        FATAL("TASK", 0x01);
    }

    ENTRY_CRITICAL();
    task_list[lt_task_table_size++] = *task;
    EXIT_CRITICAL();
}

void task_create_table(lt_task_t* app_task_table) {
    uint8_t size = 0;
    while (app_task_table[size].task_handler != (pf_task)0){
        size++;
    }

    if ((lt_task_table_size + size) > TASK_ACTIVE_OBJECT_MAX_SIZE) {
        FATAL("TASK", 0x02);
    }

    for (uint8_t i = 0; i < size; i++) {
        task_create(&app_task_table[i]);
    }
}

void task_post(task_id_t task_id, lt_msg_t* msg) {
    lt_tcb_t* tcb;

    if (task_id >= lt_task_table_size) {
        FATAL("TASK", 0x03);
    }

    ENTRY_CRITICAL();

    tcb = &active_object_task_queue[task_list[task_id].priority];
    
    /* assign the task_id to message */
    msg->next = LT_MSG_NULL;
    msg->src_task_id = current_task_info.task_id;
    msg->des_task_id = task_id;

    if (tcb->qtail == LT_MSG_NULL) {
        /* put message to queue */
        tcb->qtail = msg;
        tcb->qhead = msg;

        /* change status task to ready */
        task_ready |= tcb->mask;
    }
    else {
        /* put message to queue */
        tcb->qtail->next = msg;
        tcb->qtail = msg;
    }

    EXIT_CRITICAL();
}

void task_post_pure_msg(task_id_t des_task_id, uint8_t signal) {
    lt_msg_t* get_pure_msg_from_pool = get_pure_msg();
    get_pure_msg_from_pool->signal = signal;
    task_post(des_task_id, get_pure_msg_from_pool);
}

void task_post_common_msg(task_id_t des_task_id, uint8_t signal, uint8_t* data, uint8_t len) {
    lt_msg_t* get_common_msg_from_pool = get_common_msg();
    get_common_msg_from_pool->signal = signal;
    set_data_common_msg(get_common_msg_from_pool, data, len);
    task_post(des_task_id, get_common_msg_from_pool);
}

void task_post_dynamic_msg(task_id_t des_task_id, uint8_t signal, uint8_t* data, uint32_t len) {
    lt_msg_t* get_dynamic_msg_from_pool = get_dynamic_msg();
    get_dynamic_msg_from_pool->signal = signal;
    set_data_dynamic_msg(get_dynamic_msg_from_pool, data, len);
    task_post(des_task_id, get_dynamic_msg_from_pool);
}

void task_scheduler() {
    ENTRY_CRITICAL();

    uint8_t task_exe;

    while ((task_exe = LOG2LKUP(task_ready)) > 0) {
        /* get task execute */
        lt_tcb_t* tcb = &active_object_task_queue[task_exe - 1];

        /* get message from queue */
        lt_msg_t* msg_exe = tcb->qhead;
        
        /* move head message queue to new position */
        tcb->qhead = msg_exe->next;

        if (msg_exe->next == LT_MSG_NULL) {
            tcb->qtail = LT_MSG_NULL;

            /* change status of task to inactive */
            task_ready &= ~tcb->mask;
        }

        /* update current active object */
        memcpy((lt_task_t*)&current_task_info, &task_list[msg_exe->des_task_id], sizeof(lt_task_t));
        memcpy((lt_msg_t*)&current_active_object, msg_exe, sizeof(lt_msg_t));

        EXIT_CRITICAL();

        /* execute task */
        task_list[msg_exe->des_task_id].task_handler(msg_exe);

        /* free message to pool memory */
        free_msg(msg_exe);
    }

    EXIT_CRITICAL();
}

void task_polling_create(lt_task_polling_t* task_polling) {
    if (task_polling_size >= TASK_POLLING_MAX_SIZE) {
        FATAL("TASK", 0x04);
    }

    ENTRY_CRITICAL();
    task_polling_list[task_polling_size++] = *task_polling;
    EXIT_CRITICAL();
}

void task_polling_create_table(lt_task_polling_t* task_polling_table) {
    uint8_t size = 0;

    while (task_polling_table[size].task_polling_handler != (pf_task_polling)0) {
        size++;
    }
    
    if ((task_polling_size + size) > TASK_POLLING_MAX_SIZE) {
        FATAL("TASK", 0x05);
    }

    for (uint8_t i = 0; i < size; i++) {
        task_polling_create(&task_polling_table[i]);
    }
}

void task_polling_run() {
    for (uint8_t i = 0; i < task_polling_size; i++) {
        ENTRY_CRITICAL();
        if (task_polling_list[i].ability == LT_ENABLE) {
            EXIT_CRITICAL();
            task_polling_list[i].task_polling_handler();
        }
        else {
            EXIT_CRITICAL();
        }
    }
}

void task_polling_set_ability(task_id_t task_polling_id, uint8_t ability) {
    for (uint8_t i = 0; i < task_polling_size; i++) {
        if (task_polling_list[i].id == task_polling_id) {
            ENTRY_CRITICAL();
            task_polling_list[i].ability = ability;
            EXIT_CRITICAL();
            return;
        }
    }

    FATAL("TASK", 0x06);
}

void task_init_info() {
    LT_LOG("\n");
    LT_LOG("task init information:\n");
    LT_LOG("[active_obj]\n");
    for (uint8_t i = 0; i < lt_task_table_size; i++) {
        LT_LOG("task id: %d \ttask pri: %d \ttask info: %s\n", task_list[i].task_id, task_list[i].priority, task_list[i].info);
    }
    LT_LOG("\n");
    LT_LOG("[polling_obj]\n");
    for (uint8_t i = 0; i < task_polling_size; i++) {
        LT_LOG("task id: %d \tinitial: %d \ttask info: %s\n", task_polling_list[i].id, task_polling_list[i].ability, task_polling_list[i].info);
    }
    LT_LOG("\n");
}

int task_run() {
    for (;;) {
        task_scheduler();
        task_polling_run();
    }
}

lt_task_t* get_current_task_info() {
    return (lt_task_t*)&current_task_info;
}

lt_msg_t* get_current_active_object() {
    return (lt_msg_t*)&current_active_object;
}
