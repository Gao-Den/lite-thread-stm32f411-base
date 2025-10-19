/**
 ******************************************************************************
 * @author: GaoDen
 * @date:   03/05/2024
 ******************************************************************************
**/

#include "lt_message.h"

#include "lt_log.h"
#include "lt_common.h"

/* pure message pool memory */
static lt_pure_msg_t pure_msg_pool[LT_PURE_MSG_POOL_SIZE];
static lt_msg_t* free_list_pure_msg_pool;
static uint32_t free_list_pure_msg_used;

/* common message pool memory */
static lt_common_msg_t common_msg_pool[LT_COMMON_MSG_POOL_SIZE];
static lt_msg_t* free_list_common_msg_pool;
static uint32_t free_list_common_used;

/* dynamic message pool memory */
static lt_dynamic_msg_t dynamic_msg_pool[LT_DYNAMIC_MSG_POOL_SIZE];
static lt_msg_t* free_list_dynamic_msg_pool;
static uint32_t free_list_dynamic_msg_used;

/* messages init */
static void pure_msg_init();
static void common_msg_init();
static void dynamic_msg_init();

/* memory utility */
void* lt_malloc(size_t size);
void lt_malloc_free(void* ptr);

/*****************************************************************************
 * pure message function
 *****************************************************************************/
void pure_msg_init() {

    ENTRY_CRITICAL();

    free_list_pure_msg_pool = (lt_msg_t*)pure_msg_pool;
     
    for (uint32_t index = 0; index < LT_PURE_MSG_POOL_SIZE; index++) {
        pure_msg_pool[index].msg_header.msg_type = PURE_MSG_TYPE;
        if (index == (LT_PURE_MSG_POOL_SIZE - 1)) {
            pure_msg_pool[index].msg_header.next = LT_MSG_NULL;
        }
        else {
            pure_msg_pool[index].msg_header.next = (lt_msg_t*)&pure_msg_pool[index + 1];
        }
    }

    free_list_pure_msg_used = 0;

    EXIT_CRITICAL();
}

lt_msg_t* get_pure_msg() {
    lt_msg_t* get_msg;

    ENTRY_CRITICAL();

    /* assign the free message list to new allocate message */
    get_msg = free_list_pure_msg_pool;

    if (get_msg == LT_MSG_NULL) {
        FATAL("MSG", 0x01);
    }

    /* move the free message list to new position */
    free_list_pure_msg_pool = get_msg->next;
    free_list_pure_msg_used++;
    
    EXIT_CRITICAL();

    return get_msg;
}

void free_pure_msg(lt_msg_t* msg) {
    ENTRY_CRITICAL();

    /* move the current message to front free message list */
    msg->next = free_list_pure_msg_pool;

    /* move the free message list to head */
    free_list_pure_msg_pool = msg;
    free_list_pure_msg_used--;

    EXIT_CRITICAL();
}

uint8_t get_pure_msg_free() {
    return (LT_PURE_MSG_POOL_SIZE - free_list_pure_msg_used);
}

/*****************************************************************************
 * common message function
 *****************************************************************************/
void common_msg_init() {

    ENTRY_CRITICAL();

    free_list_common_msg_pool = (lt_msg_t*)common_msg_pool;

    for (uint32_t index = 0; index < LT_COMMON_MSG_POOL_SIZE; index++) {
        common_msg_pool[index].msg_header.msg_type = COMMON_MSG_TYPE;
        if (index == (LT_COMMON_MSG_POOL_SIZE - 1)) {
            common_msg_pool[index].msg_header.next = LT_MSG_NULL;
        }
        else {
            common_msg_pool[index].msg_header.next = (lt_msg_t*)&common_msg_pool[index + 1];
        }
    }

    free_list_common_used = 0;

    EXIT_CRITICAL();
}

lt_msg_t* get_common_msg() {
    lt_msg_t* get_msg;

    ENTRY_CRITICAL();

    /* assign the free message list to new allocate message */
    get_msg = free_list_common_msg_pool;

    if (get_msg == LT_MSG_NULL) {
        FATAL("MSG", 0x02);
    }

    /* move the free message list to new position */
    free_list_common_msg_pool = get_msg->next;
    ((lt_common_msg_t*)get_msg)->data_size = 0;
    free_list_common_used++;

    EXIT_CRITICAL();

    return get_msg;
}

void set_data_common_msg(lt_msg_t* msg, uint8_t* data, uint8_t size) {
    if (msg->msg_type != COMMON_MSG_TYPE) {
        FATAL("MSG", 0x03);
    }

    if (size > LT_COMMON_MSG_DATA_SIZE) {
        FATAL("MSG", 0x04);
    }

    ((lt_common_msg_t*)msg)->data_size = size;
    memcpy(((lt_common_msg_t*)msg)->data, data, size);
}

uint8_t* get_data_common_msg(lt_msg_t* msg) {
    if (msg->msg_type != COMMON_MSG_TYPE) {
        FATAL("MSG", 0x05);
    }

    return ((lt_common_msg_t*)msg)->data;
}

void free_common_msg(lt_msg_t* msg) {
    ENTRY_CRITICAL();

    /* move the current message to front free message list */
    msg->next = free_list_common_msg_pool;

    /* move the free message list to head */
    free_list_common_msg_pool = msg;
    free_list_common_used--;

    EXIT_CRITICAL();
}

uint8_t get_common_msg_free() {
    return (LT_COMMON_MSG_POOL_SIZE - free_list_common_used);
}

/*****************************************************************************
 * dynamic message function
 *****************************************************************************/
void dynamic_msg_init() {
    ENTRY_CRITICAL();

    free_list_dynamic_msg_pool = (lt_msg_t*)dynamic_msg_pool;

    for (uint32_t index = 0; index < LT_DYNAMIC_MSG_POOL_SIZE; index++) {
        dynamic_msg_pool[index].msg_header.msg_type = DYNAMIC_MSG_TYPE;
        if (index == (LT_DYNAMIC_MSG_POOL_SIZE - 1)) {
            dynamic_msg_pool[index].msg_header.next = LT_MSG_NULL;
        }
        else {
            dynamic_msg_pool[index].msg_header.next = (lt_msg_t*)&dynamic_msg_pool[index + 1];
        }
    }
    
    free_list_dynamic_msg_used = 0;

    EXIT_CRITICAL();
}

lt_msg_t* get_dynamic_msg() {
    lt_msg_t* get_msg;

    ENTRY_CRITICAL();

    /* assign the free message list to new allocate message */
    get_msg = free_list_dynamic_msg_pool;

    if (get_msg == LT_MSG_NULL) {
        FATAL("MSG", 0x06);
    }

    /* move the free message list to new position */
    free_list_dynamic_msg_pool = get_msg->next;
	((lt_dynamic_msg_t*)get_msg)->data_size = 0;
	((lt_dynamic_msg_t*)get_msg)->data = ((uint8_t*)0);
    free_list_dynamic_msg_used++;

    EXIT_CRITICAL();

    return get_msg;
}

void set_data_dynamic_msg(lt_msg_t* msg, uint8_t* data, uint32_t size) {
    if (msg->msg_type != DYNAMIC_MSG_TYPE) {
        FATAL("MSG", 0x07);
    }

    ((lt_dynamic_msg_t*)msg)->data_size = size;
    ((lt_dynamic_msg_t*)msg)->data = (uint8_t*)lt_malloc(size);
    memcpy(((lt_dynamic_msg_t*)msg)->data, data, size);
}

uint8_t* get_data_dynamic_msg(lt_msg_t* msg) {
    if (msg->msg_type != DYNAMIC_MSG_TYPE) {
        FATAL("MSG", 0x08);
    }

    return ((lt_dynamic_msg_t*)msg)->data;
}

void free_dynamic_msg(lt_msg_t* msg) {
    ENTRY_CRITICAL();

    /* move the current message to front free message list */
    msg->next = free_list_dynamic_msg_pool;

    /* move the free message list to head */
    free_list_dynamic_msg_pool = msg;
    free_list_dynamic_msg_used--;

    /* free memory to heap section */
    lt_malloc_free(((lt_dynamic_msg_t*)msg)->data);

    EXIT_CRITICAL();
}

uint8_t get_dynamic_msg_free() {
    return (LT_DYNAMIC_MSG_POOL_SIZE - free_list_dynamic_msg_used);
}

/*****************************************************************************
 * message utility function
 *****************************************************************************/
void msg_init() {
    pure_msg_init();
    common_msg_init();
    dynamic_msg_init();
    LT_LOG_KERNEL("[message] messsage pool memory initialized successfully\n");
}

void free_msg(lt_msg_t* msg) {
    switch (msg->msg_type) {
    case PURE_MSG_TYPE: {
        free_pure_msg(msg);
    }
        break;
    
    case COMMON_MSG_TYPE: {
        free_common_msg(msg);
    }
        break;

    case DYNAMIC_MSG_TYPE: {
        free_dynamic_msg(msg);
    }
        break;

    default: {
        FATAL("MSG", 0xFE);
    }
        break;
    }
}

void* lt_malloc(size_t size) {
    extern uint32_t __heap_end__;
    static uint8_t* lt_mem_addr = NULL;

    if (lt_mem_addr != NULL) {
        if (((uint32_t)lt_mem_addr + size) > ((uint32_t)&__heap_end__)) {
            FATAL("MEM", 0x01);
        }
    }

    lt_mem_addr = malloc(size);

    if (lt_mem_addr == NULL) {
        FATAL("MEM", 0x02);
    }

    return lt_mem_addr;
}

void lt_malloc_free(void* ptr) {
    free(ptr);
}
