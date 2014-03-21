/*
 * This file is part of the libopencm3 project.
 *
 * Copyright (C) 2009 Uwe Hermann <uwe@hermann-uwe.de>
 * Copyright (C) 2011 Stephen Caudle <scaudle@doceme.com>
 * Copyright (C) 2012 Karl Palsson <karlp@tweak.net.au>
 *
 * This library is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/timer.h>

#define PORT_LED GPIOA
#define PIN_LED GPIO7

static void gpio_setup(void)
{
	/* Enable GPIOB clock. */
	/* Manually: */
	//RCC_AHBENR |= RCC_AHBENR_GPIOCEN;
	/* Using API functions: */
    rcc_clock_setup_in_hse_8mhz_out_48mhz();
    //rcc_clock_setup_in_hsi_out_8mhz();
	rcc_periph_clock_enable(RCC_GPIOA);


	/* Set GPIO6 (in GPIO port B) to 'output push-pull'. */
	/* Using API functions: */
    gpio_mode_setup(PORT_LED, GPIO_MODE_AF,
                        GPIO_OTYPE_PP, PIN_LED);
    gpio_set_output_options(PORT_LED, GPIO_OTYPE_PP, GPIO_OSPEED_HIGH,
        PIN_LED);
    gpio_set_af(GPIOA, GPIO_AF1, GPIO7);

    rcc_periph_clock_enable(RCC_TIM3);                  
    timer_reset(TIM3);
    timer_set_mode(TIM3, TIM_CR1_CKD_CK_INT,
        TIM_CR1_CMS_EDGE, TIM_CR1_DIR_UP);
    int prescaler = (uint16_t) ((48000000 / 2) / 21000000) - 1;

    timer_set_prescaler(TIM3, prescaler);
    //timer_set_repetition_counter(TIM3, 0);
    timer_enable_preload(TIM3);
    timer_continuous_mode(TIM3);
    timer_set_period(TIM3, 665);
    timer_disable_oc_output(TIM3, TIM_OC2);
    timer_set_oc_mode(TIM3, TIM_OC2, TIM_OCM_PWM1);
    timer_set_oc_value(TIM3, TIM_OC2, 500);
    timer_set_oc_polarity_high(TIM3, TIM_OC2);
    timer_enable_oc_output(TIM3, TIM_OC2);
    timer_enable_preload_complementry_enable_bits(TIM3);
    timer_enable_counter(TIM3);
}

int main(void)
{
	volatile int i;
    volatile int brightness = 0;
    volatile int up = 1;

	gpio_setup();

    /* Infinite loop */
    while (1)  // Do not exit
    {
        if (up) {
            brightness++;
        } else {
            brightness--;
        }

        if (brightness >= 700) {
            up = 0;
        } else if (brightness <= 0) {
            up = 1;
        }

        timer_set_oc_value(TIM3, TIM_OC2, brightness);

        for (i = 0; i < 2500; i++)
            ;  // delay
    }
	return 0;
}
