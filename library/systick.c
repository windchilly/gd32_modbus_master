#include "gd32f30x.h"
#include "systick.h"
#include "cd_timer.h"

void systick_config(void)
{
    /* setup systick timer for 1000Hz interrupts */
    if (SysTick_Config(SystemCoreClock / 1000U)){
        /* capture error */
        while (1){
        }
    }
    /* configure the systick handler priority */
    NVIC_SetPriority(SysTick_IRQn, 0x00U);
}

void SysTick_Handler(void)
{
	cd_timer_update();

	return;
}
