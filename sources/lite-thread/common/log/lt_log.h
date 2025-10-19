/**
 ******************************************************************************
 * @author: GaoDen
 * @date:   27/05/2025
 ******************************************************************************
**/

#ifndef __LT_LOG_H__
#define __LT_LOG_H__

#ifdef __cplusplus
 extern "C" {
#endif

#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "lt_config.h"
#include "lt_task.h"

#define FATAL(t, i)                 lt_fatal_log(t, i)

typedef void (*pf_log_flash_read)(uint32_t address, uint8_t* pbuf, uint32_t len);
typedef void (*pf_log_flash_write)(uint32_t address, uint8_t* pbuf, uint32_t len);
typedef void (*pf_log_flash_erase)();

typedef struct {
    /* fatal header */
    uint8_t type[8];
    uint8_t code;

    /* task info */
    task_id_t src_task_id;
    task_id_t des_task_id;

    /* message info */
    uint8_t msg_type;
    uint8_t signal;
} lt_log_fatal_t;

extern void lt_log_init(pf_log_flash_read flash_read, pf_log_flash_write flash_write, pf_log_flash_erase flash_erase, uint32_t log_address);
extern void lt_fatal_log(const char* type, uint8_t code);
extern void lt_fatal_log_dump();
extern void lt_fatal_log_reset();

#ifdef __cplusplus
}
#endif

#endif /* __LT_LOG_H__ */
