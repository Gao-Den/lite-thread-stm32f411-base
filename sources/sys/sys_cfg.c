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
#include "system_stm32f4xx.h"
#include "stm32f4xx_iwdg.h"

#include "sys_irq.h"
#include "lt_common.h"
#include "io_cfg.h"

/*****************************************************************************/
/* linker variable
******************************************************************************/
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
	/* enable the HSI oscillator */
	RCC_HSICmd(ENABLE);
	while(RCC_GetFlagStatus(RCC_FLAG_HSIRDY) == RESET);

	SystemCoreClockUpdate();

	/* NVIC configuration */
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
}

void sys_cfg_tick() {
	/* nvic init struct */
	NVIC_InitTypeDef NVIC_InitStructure;
	volatile uint32_t ticks = SystemCoreClock / 1000;
	
	NVIC_InitStructure.NVIC_IRQChannel = SysTick_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = SYS_IRQ_PRIO_SYSTEM_TICK;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	/* system tick config */
    if (SysTick_Config(ticks)) { 
        FATAL("TICK", 0x01);
    }
}

/******************************************************************************
* system memory function
*******************************************************************************/
static uint32_t sys_stack_size;

uint32_t sys_stack_fill() {
	volatile uint32_t* start_stack = &_sstack;
	uint32_t* current_stack = (uint32_t*)((uint32_t)__get_MSP());

	sys_stack_size = ((uint32_t)&_estack - (uint32_t)&_sstack);

	__disable_irq();
	while (start_stack < current_stack) {
		*start_stack++ = 0xDEADBEEBu;
	}
	__enable_irq();

	return sys_stack_size;
}

uint32_t sys_stack_used() {
	volatile uint32_t* p_stack_check = &_sstack;
	while (*p_stack_check == 0xDEADBEEBu) {
		*p_stack_check++;
	}
	return ((uint32_t)&_estack - (uint32_t)p_stack_check);
}

uint32_t sys_stack_get_size() {
	return sys_stack_size;
}

/******************************************************************************
* system utility function
*******************************************************************************/
void sys_ctrl_delay_us(uint32_t us) {
    volatile uint32_t current_ticks = SysTick->VAL;

	/* number of ticks per millisecond */
	const uint32_t tick_per_ms = SysTick->LOAD + 1;

	/* number of ticks need to count */
	const uint32_t number_of_tick = ((us - ((us > 0) ? 1 : 0)) * tick_per_ms) / 1000;

	/* number of elapsed ticks */
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
	IWDG_SetPrescaler(IWDG_Prescaler_256);		/* iwdg counter clock: 37KHz (LSI) / 256 = 0.144 KHz */
	IWDG_SetReload(0xFFFF);						/* set counter reload, T = (1 / iwdg counter clock) * reload_counter  ~30s  */
	IWDG_ReloadCounter();
	IWDG_Enable();
}

void sys_ctrl_independent_watchdog_reset() {
	__disable_irq();
	IWDG_ReloadCounter();
	__enable_irq();
}
