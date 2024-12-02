#include "gd32f30x.h"
#include "systick.h"

int main(void)
{
    /* configure systick */
    systick_config();
    
    /* enable the LEDs GPIO clock */
    rcu_periph_clock_enable(RCU_GPIOB);

    /* configure LED1 LED2 GPIO port */
    gpio_init(GPIOB, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_14);
    gpio_init(GPIOB, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_15);
    
    /* reset LED1 LED2 GPIO pin */
    gpio_bit_reset(GPIOB, GPIO_PIN_14);
    gpio_bit_reset(GPIOB, GPIO_PIN_15);

    while(1)
	{
        gpio_bit_set(GPIOB, GPIO_PIN_14);
        gpio_bit_set(GPIOB, GPIO_PIN_15);
        gpio_bit_reset(GPIOB, GPIO_PIN_14);
        gpio_bit_reset(GPIOB, GPIO_PIN_15);
    }
}
