/**
 ******************************************************************************
 * @author: GaoDen
 * @date:   18/11/2024
 ******************************************************************************
**/

#include "sys_startup.h"

#include "lt_task.h"
#include "lt_message.h"
#include "lt_console.h"
#include "lt_common.h"

#include "misc.h"
#include "system_stm32f4xx.h"

#include "sys_irq.h"
#include "sys_cfg.h"
#include "sys_ctrl.h"

#include "io_cfg.h"
#include "lt_common.h"
#include "led.h"

#include "app.h"
#include "task_list.h"
#include "task_dbg.h"
#include "task_console.h"
#include "task_life.h"

/* define the addresses of data and bss sections */
extern uint32_t _sidata, _sdata, _edata, _sbss, _ebss, _estack;
extern void (*__preinit_array_start[])();
extern void (*__preinit_array_end[])();
extern void (*__init_array_start[])();
extern void (*__init_array_end[])();

/* reset handler */
void reset_handler(void) {
    /* MUST BE disable irq */
    __disable_irq();

    /* fill stack section */
    sys_stack_fill();

    /* call the system init function */
    SystemInit();

    uint32_t *src, *dest;

    /*****************************************************************************/
    /* copy the data segment initializers from flash to SRAM
    ******************************************************************************/
    /* pointer start address in flash */
    src = &_sidata;

    /* pointer start address in sram */
    dest = &_sdata;

    while (dest < &_edata) {
        *dest++ = *src++;
    }

    /* zero fill to .bss section */
    dest = &_sbss;
    while (dest < &_ebss) {
        *dest++ = 0;
    }

    /*****************************************************************************/
    /* system config
    ******************************************************************************/
    sys_cfg_common();
    sys_cfg_clock();
    sys_cfg_tick();

    volatile unsigned i, cnt;
    
	/* invoke all static constructors */
	cnt = __preinit_array_end - __preinit_array_start;
	for (i = 0; i < cnt; i++) {
        __preinit_array_start[i]();
    }
		
	cnt = __init_array_end - __init_array_start;
	for (i = 0; i < cnt; i++) {
        __init_array_start[i]();
    }
    
    /* io init */
    io_init();

    __enable_irq();

    /* go to the main application */
    app();
}

/* interrupt vector table */
__attribute__((section(".isr_vector"))) void (*const g_pfnVectors[])(void) = {
    /* system interrupt */
    (void (*)(void))&_estack,                        /* stack pointer */
    reset_handler,                                   /* reset handler */
    nmi_handler,                                     /* nmi handler */
    hardfault_handler,                               /* hard fault handler */
    mem_manage_handler,                              /* mem manage handler */
    bus_fault_handler,                               /* bus fault handler */
    usage_fault_handler,                             /* usage fault handler */
    0,                                               /* reserved */
    0,                                               /* reserved */
    0,                                               /* reserved */
    0,                                               /* reserved */
    svc_handler,                                     /* svc handler */
    debug_mon_handler,                               /* debug mon handler */
    0,                                               /* reserved */
    pendsv_handler,                                  /* pendsv handler */
    system_tick_handler,                             /* systick handler */

    /* external interrupts */
    default_handler,                                 /* window watchdog */
    default_handler,                                 /* pvd through exti line detection */
    default_handler,                                 /* tamper and timestamps through the exti line */
    default_handler,                                 /* rtc wakeup through the exti line */
    default_handler,                                 /* flash */
    default_handler,                                 /* rcc */
    default_handler,                                 /* exti line0 */
    default_handler,                                 /* exti line1 */
    default_handler,                                 /* exti line2 */
    default_handler,                                 /* exti line3 */
    default_handler,                                 /* exti line4 */
    default_handler,                                 /* dma1 stream 0 */
    default_handler,                                 /* dma1 stream 1 */
    default_handler,                                 /* dma1 stream 2 */
    default_handler,                                 /* dma1 stream 3 */
    default_handler,                                 /* dma1 stream 4 */
    default_handler,                                 /* dma1 stream 5 */
    default_handler,                                 /* dma1 stream 6 */
    default_handler,                                 /* adc */
    default_handler,                                 /* reserved */
    default_handler,                                 /* reserved */
    default_handler,                                 /* reserved */
    default_handler,                                 /* reserved */
    exti8_handler,                                   /* exti9_5 */
    default_handler,                                 /* tim1 break and tim9 */
    default_handler,                                 /* tim1 update and tim10 */
    default_handler,                                 /* tim1 trigger and commutation and tim11 */
    default_handler,                                 /* tim1 capture compare */
    default_handler,                                 /* tim2 */
    default_handler,                                 /* tim3 */
    default_handler,                                 /* tim4 */
    default_handler,                                 /* i2c1 event */
    default_handler,                                 /* i2c1 error */
    default_handler,                                 /* i2c2 event */
    default_handler,                                 /* i2c2 error */
    default_handler,                                 /* spi1 */
    default_handler,                                 /* spi2 */
    usart1_irq_handler,                              /* usart1 */
    default_handler,                                 /* usart2 */
    default_handler,                                 /* reserved */
    default_handler,                                 /* exti15_10 */
    default_handler,                                 /* rtc alarm through exti line */
    default_handler,                                 /* usb otg fs wakeup through exti line */
    default_handler,                                 /* reserved */
    default_handler,                                 /* reserved */
    default_handler,                                 /* reserved */
    default_handler,                                 /* reserved */
    default_handler,                                 /* dma1 stream 7 */
    default_handler,                                 /* reserved */
    default_handler,                                 /* sdio */
    default_handler,                                 /* tim5 */
    default_handler,                                 /* spi3 */
    default_handler,                                 /* reserved */
    default_handler,                                 /* reserved */
    default_handler,                                 /* reserved */
    default_handler,                                 /* reserved */
    default_handler,                                 /* dma2 stream 0 */
    default_handler,                                 /* dma2 stream 1 */
    default_handler,                                 /* dma2 stream 2 */
    default_handler,                                 /* dma2 stream 3 */
    default_handler,                                 /* dma2 stream 4 */
    default_handler,                                 /* reserved */
    default_handler,                                 /* reserved */
    default_handler,                                 /* reserved */
    default_handler,                                 /* reserved */
    default_handler,                                 /* reserved */
    default_handler,                                 /* reserved */
    default_handler,                                 /* usb otg fs */
    default_handler,                                 /* dma2 stream 5 */
    default_handler,                                 /* dma2 stream 6 */
    default_handler,                                 /* dma2 stream 7 */
    default_handler,                                 /* usart6 */
    default_handler,                                 /* i2c3 event */
    default_handler,                                 /* i2c3 error */
    default_handler,                                 /* reserved */
    default_handler,                                 /* reserved */
    default_handler,                                 /* reserved */
    default_handler,                                 /* reserved */
    default_handler,                                 /* reserved */
    default_handler,                                 /* reserved */
    default_handler,                                 /* reserved */
    default_handler,                                 /* fpu */
    default_handler,                                 /* reserved */
    default_handler,                                 /* reserved */
    default_handler,                                 /* spi4 */
    default_handler,                                 /* spi5 */
};
