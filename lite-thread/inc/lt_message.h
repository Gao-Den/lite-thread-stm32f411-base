/**
 ******************************************************************************
 * @author: GaoDen
 * @date:   03/05/2024
 ******************************************************************************
**/

#ifndef __LT_MESSAGE_H__
#define __LT_MESSAGE_H__

#ifdef __cplusplus
 extern "C" {
#endif 

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

#include "lt_config.h"

#define LT_MSG_NULL                    ((lt_msg_t*)0)

typedef struct lt_msg_t {
    /* message management - private for kernel */
    struct lt_msg_t* next;
    uint8_t msg_type;

    /* message header - public for user application */
    uint8_t src_task_id;
    uint8_t des_task_id;
    uint8_t signal;

    /* link layer */
    uint8_t if_src_task_id;
    uint8_t if_des_task_id;
    uint8_t if_signal;
} lt_msg_t;

enum {
    SHELL_CMD_PARSER,
    USER_DEFINE_APP_SIGNAL,
};

/******************************************************************************
* message define 
*******************************************************************************/
#define PURE_MSG_TYPE 					(0x01)
#define COMMON_MSG_TYPE                 (0x02)
#define DYNAMIC_MSG_TYPE                (0x04)

extern void msg_init();
extern void free_msg(lt_msg_t* msg);

/******************************************************************************
* pure message define
*******************************************************************************/
typedef struct {
    lt_msg_t msg_header;
} lt_pure_msg_t;

extern lt_msg_t* get_pure_msg();
extern uint8_t get_pure_msg_free();

/******************************************************************************
* common message define
*******************************************************************************/
typedef struct {
    lt_msg_t msg_header;
    uint8_t data_size;
    uint8_t data[LT_COMMON_MSG_DATA_SIZE];
} lt_common_msg_t;

extern lt_msg_t* get_common_msg();
extern void set_data_common_msg(lt_msg_t* msg, uint8_t* data, uint8_t size);
extern uint8_t* get_data_common_msg(lt_msg_t* msg);
extern uint8_t get_common_msg_free();

/******************************************************************************
* dynamic message define
*******************************************************************************/
typedef struct {
    lt_msg_t msg_header;
    uint8_t data_size;
    uint8_t* data;
} lt_dynamic_msg_t;

extern lt_msg_t* get_dynamic_msg();
extern void set_data_dynamic_msg(lt_msg_t* msg, uint8_t* data, uint32_t size);
extern uint8_t* get_data_dynamic_msg(lt_msg_t* msg);
extern uint8_t get_dynamic_msg_free();

/******************************************************************************
* dynamic memory allocate
*******************************************************************************/
extern void* lt_malloc(size_t size);
extern void lt_malloc_free(void* ptr);

#ifdef __cplusplus
}
#endif

#endif /* __LT_MESSAGE_H__ */
