/**
 ******************************************************************************
 * @author: GaoDen
 * @date:   27/05/2025
 ******************************************************************************
**/

#ifndef __LT_CONSOLE_H__
#define __LT_CONSOLE_H__

#ifdef __cplusplus
 extern "C" {
#endif

#include <stdint.h>
#include <stdio.h>

#include "xprintf.h"
#include "lt_config.h"

#define LT_LOG(fmt, ...)                    xprintf(fmt, ##__VA_ARGS__)
#define LT_LOGE(fmt, ...)                   xprintf("[ERROR] " fmt, ##__VA_ARGS__)

#if (LT_LOG_KERNEL_EN == 1)
    #define LT_LOG_KERNEL(fmt, ...)         LT_LOG("[KERNEL] " fmt, ##__VA_ARGS__)
#else
    #define LT_LOG_KERNEL(fmt, ...)
#endif
#if (LT_LOG_TASK_EN == 1)
    #define LT_LOG_TASK(fmt, ...)           LT_LOG("[TASK] " fmt, ##__VA_ARGS__)
#else
    #define LT_LOG_TASK(fmt, ...)
#endif
#if (LT_LOG_TIMER_EN == 1)
    #define LT_LOG_TIMER(fmt, ...)          LT_LOG("[TIMER] " fmt, ##__VA_ARGS__)
#else
    #define LT_LOG_TIMER(fmt, ...)
#endif
#if (LT_LOG_MESSAGE_EN == 1)
    #define LT_LOG_MESSAGE(fmt, ...)        LT_LOG("[MESSAGE] " fmt, ##__VA_ARGS__)
#else
    #define LT_LOG_MESSAGE(fmt, ...)
#endif

typedef void (*pf_console_putc)(uint8_t);

extern void lt_console_init(pf_console_putc pf_putc);

#ifdef __cplusplus
}
#endif

#endif /* __LT_CONSOLE_H__ */
