/**
 ******************************************************************************
 * @author: GaoDen
 * @date:   27/05/2025
 ******************************************************************************
**/

#ifndef __APP_FLASH_H__
#define __APP_FLASH_H__

#ifdef __cplusplus
 extern "C" {
#endif 

#include <stdint.h>

#define WINBOND_SECTOR_SIZE                   (4UL * 1024UL)

#define APP_FLASH_LT_LOG_OBJ_ADDR             (0x00000000UL)
#define APP_FLASH_APP_LOG_OBJ_ADDR            (APP_FLASH_LT_LOG_OBJ_ADDR + WINBOND_SECTOR_SIZE)

#ifdef __cplusplus
}
#endif

#endif /* __APP_FLASH_H__ */
