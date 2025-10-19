/**
 ******************************************************************************
 * @author: GaoDen
 * @date:   18/11/2024
 ******************************************************************************
**/

#include "sys_irq.h"

#include "lt_timer.h"
#include "lt_common.h"

#include "sys_ctrl.h"
#include "system_stm32f4xx.h"

#include "io_cfg.h"
#include "app.h"
#include "app_dbg.h"
#include "task_list.h"
#include "task_console.h"

#include "nrf_phy.h"

/* counter of milliseconds since the application run */
volatile uint32_t millis_current;

void default_handler() {
	FATAL("SYSTEM", 0x01);
}

void nmi_handler() {
	FATAL("SYSTEM", 0x02);
}

void hardfault_handler() {
	FATAL("SYSTEM", 0x03);
}

void mem_manage_handler() {
	FATAL("SYSTEM", 0x04);
}

void bus_fault_handler() {
	FATAL("SYSTEM", 0x05);
}

void usage_fault_handler() {
	FATAL("SYSTEM", 0x06);
}

void svc_handler() {

}

void debug_mon_handler() {

}

void pendsv_handler() {

}

void enable_interrupts() {
    __enable_irq();
}

void disable_interrupts() {
    __disable_irq();
}

void system_tick_handler() {
	static uint32_t div_counter = 0;

	/* increasing millis counter */
	millis_current++;

	/* kernel tick */
	timer_tick(1);

	if (div_counter == 10) {
		div_counter = 0;
	}

	switch (div_counter) {
	case 0:
		sys_irq_timer_10ms();
		break;

	default:
		break;
	}

	div_counter++;
}

uint32_t sys_ctrl_millis() {
	volatile uint32_t ret;
	__disable_irq();
	ret = millis_current;
	__enable_irq();
	return ret;
}

void usart1_irq_handler() {
	volatile uint8_t c = 0;
	c = usart1_get_char();
	lt_shell_getc(c);
}

void exti8_handler() {
    if (EXTI_GetITStatus(NRF24L01_IRQ_LINE) != RESET) {
		nrf_phy_irq();

        /* clear interrupt pending bit */
        EXTI_ClearITPendingBit(NRF24L01_IRQ_LINE);
    }
}
