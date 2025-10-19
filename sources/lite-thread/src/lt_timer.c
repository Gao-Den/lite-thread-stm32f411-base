/**
 ******************************************************************************
 * @author: GaoDen
 * @date:   03/05/2024
 ******************************************************************************
**/

#include "lt_timer.h"

#include "lt_log.h"
#include "lt_common.h"

/* define message data is transfered between interrupt heart beat and timer task */
struct lt_timer_irq_t {
    uint32_t counter;
    uint32_t enable;
};

static volatile struct lt_timer_irq_t lt_timer_irq = {0, LT_DISABLE};

/* timer pool management */
static lt_timer_t timer_pool[LT_TIMER_POOL_SIZE];
static lt_timer_t* free_list_timer_pool;
static uint32_t free_list_timer_used;
static lt_timer_t* timer_list_head;

/* timer pool allocate - free memory */
static void timer_pool_init();
static lt_timer_t* get_timer_msg();
static void free_timer_msg(lt_timer_t* timer_msg);

lt_task_t task_timer_tick;

void timer_init() {
    timer_pool_init();

    ENTRY_CRITICAL();

    lt_timer_irq.counter = 0;
    lt_timer_irq.enable = LT_ENABLE;

    EXIT_CRITICAL();

    task_timer_tick.task_id = TASK_TIMER_TICK_ID;
    task_timer_tick.priority = TASK_PRIORITY_LEVEL_7;
    task_timer_tick.task_handler = lt_task_timer_handler;
    task_timer_tick.info = (const uint8_t*)"task timer tick";
    task_create(&task_timer_tick);

    LT_LOG_KERNEL("[timer] timer service initialized successfully\n");
}

void timer_pool_init() {
    ENTRY_CRITICAL();

    free_list_timer_pool = (lt_timer_t*)timer_pool;
    timer_list_head = LT_TIMER_NULL;

    for (uint32_t index = 0; index < LT_TIMER_POOL_SIZE; index++) {
        if (index == (LT_TIMER_POOL_SIZE - 1)) {
            timer_pool[index].next = LT_TIMER_NULL;
        }
        else {
            timer_pool[index].next = (lt_timer_t*)&timer_pool[index + 1];
        }
    }

    free_list_timer_used = 0;

    EXIT_CRITICAL();
}

lt_timer_t* get_timer_msg() {
    lt_timer_t* get_timer_pool;

    ENTRY_CRITICAL();

    get_timer_pool = free_list_timer_pool;

    if (get_timer_pool == LT_TIMER_NULL) {
        FATAL("TIMER", 0x01);
    }
    else {
        free_list_timer_pool = get_timer_pool->next;
        free_list_timer_used++;
    }

    EXIT_CRITICAL();

    return get_timer_pool;
}

void free_timer_msg(lt_timer_t* timer_msg) {
    ENTRY_CRITICAL();

    timer_msg->next = free_list_timer_pool;
    free_list_timer_pool = timer_msg;

    free_list_timer_used--;

    EXIT_CRITICAL();
}

uint8_t get_timer_pool_free() {
    return (LT_TIMER_POOL_SIZE - free_list_timer_used);
}

uint8_t timer_set(task_id_t des_task_id, timer_sig_t signal, uint32_t duty, timer_type_t timer_type) {
    lt_timer_t* timer_msg;

    ENTRY_CRITICAL();

    /* move the timer_msg to head timer list */
    timer_msg = timer_list_head;

    /* find the previous same timer_msg in the timer list */
    while (timer_msg != LT_TIMER_NULL) {
        if ((timer_msg->des_task_id == des_task_id) && (timer_msg->signal == signal)) {

            timer_msg->counter = duty;

            EXIT_CRITICAL();

            return TIMER_OK;
        }
        else {
            timer_msg = timer_msg->next;
        }
    }

    /* create the new node timer_msg */
    timer_msg = get_timer_msg();

    timer_msg->des_task_id = des_task_id;
    timer_msg->signal = signal;
    timer_msg->counter = duty;

    if (timer_type == TIMER_PERIODIC) {
        timer_msg->period = duty;
    }
    else {
        timer_msg->period = 0;
    }

    /* put the new node timer_msg to timer list */
    if (timer_list_head == LT_TIMER_NULL) {
        timer_msg->next = LT_TIMER_NULL;
        timer_list_head = timer_msg;
    }
    else {
        timer_msg->next = timer_list_head;
        timer_list_head = timer_msg;
    }

    EXIT_CRITICAL();

    return TIMER_OK;
}

uint8_t timer_remove(task_id_t des_task_id, timer_sig_t signal) {
    lt_timer_t* cur_timer_msg;
    lt_timer_t* prev_timer_msg;

    ENTRY_CRITICAL();

    /* move the current_timer_msg to head timer list */
    cur_timer_msg = timer_list_head;
    prev_timer_msg = timer_list_head;

    /* find the same timer_msg in timer list */
    while (cur_timer_msg != LT_TIMER_NULL) {
        if ((cur_timer_msg->des_task_id == des_task_id) && (cur_timer_msg->signal) == signal) {

            if (cur_timer_msg == timer_list_head) {
                timer_list_head = cur_timer_msg->next;
            }
            else {
                /* remove the current_timer_msg from timer list */
                prev_timer_msg->next = cur_timer_msg->next;
            }

            /* return the current_timer_msg to pool timer */
            free_timer_msg(cur_timer_msg);

            EXIT_CRITICAL();

            return TIMER_OK;
        }
        else {
            prev_timer_msg = cur_timer_msg;
            cur_timer_msg = cur_timer_msg->next;
        }
    }

    EXIT_CRITICAL();

    return TIMER_ERROR;  
}

void timer_tick(uint32_t tick) {
    if (timer_list_head != LT_TIMER_NULL) {

        lt_timer_irq.counter += tick;

        if (lt_timer_irq.enable == LT_ENABLE) {
            lt_timer_irq.enable = LT_DISABLE;
            lt_msg_t* msg = get_pure_msg();
            msg->signal = TIMER_TICK_SIG;
            task_post(TASK_TIMER_TICK_ID, msg);
        }
    }
}

void lt_task_timer_handler(lt_msg_t* msg) {
    lt_timer_t* cur_timer_msg;
    lt_timer_t* timer_msg_del = LT_TIMER_NULL; /* must-be assign LT_TIMER_NULL */

    uint32_t irq_counter;
    uint32_t get_counter;

    ENTRY_CRITICAL();

    cur_timer_msg = timer_list_head;
    irq_counter = lt_timer_irq.counter;

    lt_timer_irq.counter = 0;
    lt_timer_irq.enable = LT_ENABLE;

    EXIT_CRITICAL();

    switch (msg->signal) {
    case TIMER_TICK_SIG: {
        while (cur_timer_msg != LT_TIMER_NULL) {
            ENTRY_CRITICAL();
            
            if (irq_counter < (cur_timer_msg->counter)) {
                cur_timer_msg->counter -= irq_counter;
            }
            else {
                cur_timer_msg->counter = 0;
            }
            
            get_counter = cur_timer_msg->counter;

            EXIT_CRITICAL();

            if (get_counter == 0) {
                lt_msg_t* msg = get_pure_msg();
                msg->signal = cur_timer_msg->signal;
                task_post(cur_timer_msg->des_task_id, msg);
                
                ENTRY_CRITICAL();

                if (cur_timer_msg->period) {
                    cur_timer_msg->counter = cur_timer_msg->period;
                }
                else {
                    timer_msg_del = cur_timer_msg;
                }

                EXIT_CRITICAL();
            }

            cur_timer_msg = cur_timer_msg->next;

            if (timer_msg_del) {
                timer_remove(timer_msg_del->des_task_id, timer_msg_del->signal);
                timer_msg_del = LT_TIMER_NULL;
            }
        }
    }
        break;

    default: {
    }
        break;
    }
}
