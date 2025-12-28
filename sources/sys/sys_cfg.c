/**
 ******************************************************************************
 * @author: GaoDen
 * @date:   18/11/2024
 ******************************************************************************
 **/

#include "sys_cfg.h"
#include "sys_ctrl.h"

#include "lt_task.h"

#include "misc.h"
#include "stm32f4xx.h"
#include "system_stm32f4xx.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_iwdg.h"

#include "sys_irq.h"
#include "lt_common.h"
#include "io_cfg.h"

/*****************************************************************************/
/* linker variable */
/*****************************************************************************/
extern uint32_t _start_flash;
extern uint32_t _end_flash;
extern uint32_t _start_ram;
extern uint32_t _end_ram;
extern uint32_t _sdata;
extern uint32_t _edata;
extern uint32_t _sbss;
extern uint32_t _ebss;
extern uint32_t __heap_start__;
extern uint32_t __heap_end__;
extern uint32_t _sstack;
extern uint32_t _estack;

/******************************************************************************
 * system configure function
 *****************************************************************************/
void sys_cfg_common() {
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);
}

void sys_cfg_clock() {
    /* enable the hsi oscillator */
    RCC_HSICmd(ENABLE);
    while (RCC_GetFlagStatus(RCC_FLAG_HSIRDY) == RESET);

    SystemCoreClockUpdate();

    /* nvic configuration */
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
}

void sys_cfg_tick() {
    NVIC_InitTypeDef nvic_init;
    volatile uint32_t ticks = SystemCoreClock / 1000;

    nvic_init.NVIC_IRQChannel = SysTick_IRQn;
    nvic_init.NVIC_IRQChannelPreemptionPriority = SYS_IRQ_PRIO_SYSTEM_TICK;
    nvic_init.NVIC_IRQChannelSubPriority = 0;
    nvic_init.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&nvic_init);

    /* system tick config */
    if (SysTick_Config(ticks)) {
        FATAL("TICK", 0x01);
    }
}

/******************************************************************************
 * system memory function
 *****************************************************************************/
static uint32_t sys_stack_size;

uint32_t sys_stack_fill() {
    volatile uint32_t *start_stack = &_sstack;
    uint32_t *current_stack = (uint32_t *)((uint32_t)__get_MSP());

    sys_stack_size = ((uint32_t)&_estack - (uint32_t)&_sstack);

    ENTRY_CRITICAL();
    while (start_stack < current_stack) {
        *start_stack++ = 0xDEADBEEBu;
    }
    EXIT_CRITICAL();

    return sys_stack_size;
}

uint32_t sys_stack_used() {
    volatile uint32_t *p_stack_check = &_sstack;

    while (*p_stack_check == 0xDEADBEEBu) {
        p_stack_check++;
    }

    return ((uint32_t)&_estack - (uint32_t)p_stack_check);
}

uint32_t sys_stack_get_size() {
    return sys_stack_size;
}

/******************************************************************************
 * system info function
 *****************************************************************************/
system_info_t system_info;

uint8_t sys_ctrl_get_reset_reason() {
    uint8_t reason = SYS_RESET_UNKNOWN;

    /* priority-based detection */
    if (RCC_GetFlagStatus(RCC_FLAG_IWDGRST) != RESET) {
        reason = SYS_RESET_IWDG_RESET;
    }

    if (RCC_GetFlagStatus(RCC_FLAG_WWDGRST) != RESET) {
        reason = SYS_RESET_WWDG_RESET;
    }

    if (RCC_GetFlagStatus(RCC_FLAG_SFTRST) != RESET) {
        reason = SYS_RESET_SOFTWARE_RESET;
    }

    if (RCC_GetFlagStatus(RCC_FLAG_BORRST) != RESET) {
        reason = SYS_RESET_BROWN_OUT_RESET;
    }

    if (RCC_GetFlagStatus(RCC_FLAG_PORRST) != RESET) {
        reason = SYS_RESET_POWER_ON;
    }

    if (RCC_GetFlagStatus(RCC_FLAG_LPWRRST) != RESET) {
        reason = SYS_RESET_LOW_POWER_RESET;
    }

    if (RCC_GetFlagStatus(RCC_FLAG_PINRST) != RESET) {
        if (reason == SYS_RESET_UNKNOWN) {
            reason = SYS_RESET_PIN_RESET;
        }
    }

    /* clear reset flags */
    RCC_ClearFlag();

    return reason;
}

void sys_ctrl_set_reset_reason(uint8_t reason) {
    system_info.reboot_reason = reason;
}

void sys_ctrl_update_info() {
    RCC_ClocksTypeDef rcc_clocks;

    RCC_GetClocksFreq(&rcc_clocks);

    system_info.cpu_clock = rcc_clocks.HCLK_Frequency;
    system_info.tick = 1;
    system_info.console_baudrate = CONSOLE_USART_BAUDRATE;
    system_info.flash_used = ((uint32_t)&_end_flash - (uint32_t)&_start_flash) + ((uint32_t)&_edata - (uint32_t)&_sdata);
    system_info.ram_used = (uint32_t)&_estack - (uint32_t)&_start_ram;
    system_info.data_init_size = (uint32_t)&_edata - (uint32_t)&_sdata;
    system_info.data_non_init_size = (uint32_t)&_ebss - (uint32_t)&_sbss;
    system_info.stack_avail = (uint32_t)&_estack - (uint32_t)&_end_ram;
    system_info.heap_avail = (uint32_t)&__heap_end__ - (uint32_t)&__heap_start__;
    system_info.ram_other = system_info.ram_used - (system_info.heap_avail + system_info.stack_avail + system_info.data_non_init_size + system_info.data_init_size);
}

void sys_ctrl_show_info() {
    const char *reset_str[] = {
        "unknown",
        "power-on reset",
        "pin reset",
        "software reset",
        "independent watchdog",
        "window watchdog",
        "low-power reset"
        "brown out reset",
    };

    SYS_PRINT("\n\n\n");
    SYS_PRINT("system informations:\n");
    SYS_PRINT(".reset reason: %s\n", reset_str[system_info.reboot_reason]);
    SYS_PRINT(".cpu clock: %d Hz\n", system_info.cpu_clock);
    SYS_PRINT(".console baudrate: %d bps\n", system_info.console_baudrate);
    SYS_PRINT(".flash used: %d\n", system_info.flash_used);
    SYS_PRINT(".data initialized: %d\n", system_info.data_init_size);
    SYS_PRINT(".data non initialized: %d\n", system_info.data_non_init_size);
    SYS_PRINT(".stack available: %d\n", system_info.stack_avail);
    SYS_PRINT(".heap available: %d\n", system_info.heap_avail);
    SYS_PRINT("\n");
}

/******************************************************************************
 * system utility function
 *****************************************************************************/
void sys_ctrl_delay_us(uint32_t us) {
    volatile uint32_t current_ticks = SysTick->VAL;

    const uint32_t tick_per_ms = SysTick->LOAD + 1;
    const uint32_t number_of_tick = ((us - ((us > 0) ? 1 : 0)) * tick_per_ms) / 1000;

    uint32_t elapsed_tick = 0;
    volatile uint32_t prev_tick = current_ticks;

    while (number_of_tick > elapsed_tick) {
        current_ticks = SysTick->VAL;
        elapsed_tick += (prev_tick < current_ticks) ? (tick_per_ms + prev_tick - current_ticks) : (prev_tick - current_ticks);
        prev_tick = current_ticks;
    }
}

void sys_ctrl_delay_ms(uint32_t ms) {
    uint32_t start_time = sys_ctrl_millis();
    while ((sys_ctrl_millis() - start_time) < ms);
}

void sys_ctrl_reset() {
    NVIC_SystemReset();
}

void sys_ctrl_independent_watchdog_init() {
    IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);
    IWDG_SetPrescaler(IWDG_Prescaler_256);
    IWDG_SetReload(0xFFFF);
    IWDG_ReloadCounter();
    IWDG_Enable();
}

void sys_ctrl_independent_watchdog_reset() {
    ENTRY_CRITICAL();
    IWDG_ReloadCounter();
    EXIT_CRITICAL();
}
