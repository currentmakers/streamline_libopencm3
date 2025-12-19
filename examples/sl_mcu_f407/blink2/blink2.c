/*
 * Proper blink (using systick timer) for StreamLine SL_MCU_F407
 *
 * Copyright (C) 2025 CurrentMakers <lth@currentmakers.com>
 *
 */

#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/cm3/systick.h>

volatile uint32_t systick = 0;

void sys_tick_handler(void);

/* Set STM32 to 168 MHz. */
static void clock_setup(void) {

    /* The Streamline F407 use a 16 MHz crystal and it can run up to 168 MHz */
    rcc_clock_setup_pll(&rcc_hse_16mhz_3v3[RCC_CLOCK_3V3_168MHZ]);

    /* 168MHz / 8 => 21000000 counts per second */
    systick_set_clocksource(STK_CSR_CLKSOURCE_AHB_DIV8);

    /* 21000000/21000 = 1000 overflows per second - every 1ms one interrupt */
    /* SysTick interrupt every N clock pulses: set reload to N-1 */
    systick_set_reload(20999);

    systick_interrupt_enable();

    /* Start counting. */
    systick_counter_enable();

}

static void gpio_setup(void) {

    /* Enable GPIOC clock. */
    rcc_periph_clock_enable(RCC_GPIOC);

    /* Set GPIO12-15 (in GPIO port D) to 'output push-pull'. */
    gpio_mode_setup(GPIOC, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO13);
    gpio_set_output_options(GPIOC, GPIO_OTYPE_OD, GPIO_OSPEED_2MHZ, GPIO13);
}

// The sys_tick_handler should be called once every 1 ms.  Does nada
// except updating the systick variable
void sys_tick_handler(void) {
    ++systick;
};

int main(void) {

    // Setup clocks and gpio
    clock_setup();
    gpio_setup();

    // PC13 hi so LED is off by default
    gpio_set(GPIOC, GPIO13);

    // Timing for the superloop
    uint32_t now = 0, next_blink = 500;

    /* Blink the LEDs (PD12, PD13, PD14 and PD15) on the board. */
    while (1) {

        now = systick;

        if (now >= next_blink) {
            gpio_toggle(GPIOC, GPIO13);
            next_blink = now + 500;
        }

    }

    return 0;
}
