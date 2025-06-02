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

#define LT_LOG_ESCAPE                       "\033"
#define LT_LOG_YELLOW_COLOR                 LT_LOG_ESCAPE "[0;33m"
#define LT_LOG_RED_COLOR                    LT_LOG_ESCAPE "[0;31m"
#define LT_LOG_COLOR_RESET                  LT_LOG_ESCAPE "[0m"

#define LT_LOG(fmt, ...)                    lt_printf(LT_LOG_COLOR_RESET fmt, ##__VA_ARGS__)
#define LT_LOGD(fmt, ...)                   lt_printf(LT_LOG_YELLOW_COLOR fmt LT_LOG_COLOR_RESET, ##__VA_ARGS__)
#define LT_LOGE(fmt, ...)                   lt_printf(LT_LOG_RED_COLOR fmt LT_LOG_COLOR_RESET, ##__VA_ARGS__)

#define lt_printf(fmt, ...)                 xprintf(fmt, ##__VA_ARGS__)
typedef void (*pf_console_putc)(uint8_t);

extern void lt_console_init(pf_console_putc pf_putc);

#ifdef __cplusplus
}
#endif

#endif /* __LT_CONSOLE_H__ */
