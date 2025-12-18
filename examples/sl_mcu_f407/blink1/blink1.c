/*
 * First STM32World libopencm3 demo
 *
 * Copyright (C) 2009 Uwe Hermann <uwe@hermann-uwe.de>
 * Copyright (C) 2011 Damjan Marion <damjan.marion@gmail.com>
 * Copyright (C) 2011 Mark Panajotovic <marko@electrontube.org>
 * Copyright (C) 2025 CurrentMakers <lth@currentmakers.com>
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

static void gpio_setup(void) {

    /* Enable GPIOC clock. */
    rcc_periph_clock_enable(RCC_GPIOC);

    /* Set GPIO12-15 (in GPIO port D) to 'output push-pull'. */
    gpio_mode_setup(GPIOC, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO13);
    gpio_set_output_options(GPIOC, GPIO_OTYPE_OD, GPIO_OSPEED_2MHZ, GPIO13);

}

int main(void) {

    gpio_setup();

    /* Set two LEDs for wigwag effect when toggling. */
    gpio_set(GPIOC, GPIO13);

    /* Blink the LEDs (PD12, PD13, PD14 and PD15) on the board. */
    while (1) {
        /* Toggle LEDs. */
        gpio_toggle(GPIOC, GPIO13);
        for (int i = 0; i < 1000000; i++) {
            /* Wait a bit. */
            __asm__("nop");
        }
    }

    return 0;
}

