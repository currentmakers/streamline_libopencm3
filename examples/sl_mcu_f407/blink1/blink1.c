/*
 * First STM32World libopencm3 demo
 *
 * Copyright (C) 2025 CurrentMakers <lth@currentmakers.com>
 *
 */

#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>

static void gpio_setup(void) {

    /* Enable GPIOC clock. */
    rcc_periph_clock_enable(RCC_GPIOC);

    /* Set GPIO13 (in GPIO port C) to 'output open drain'. */
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

