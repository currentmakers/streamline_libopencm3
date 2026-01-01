/*
 * Proper blink (using systick timer) for StreamLine SL_MCU_F407
 *
 * Copyright (C) 2025 CurrentMakers <lth@currentmakers.com>
 *
 */

#include <libopencm3/cm3/nvic.h>
#include <libopencm3/cm3/systick.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/usart.h>

#include <sys/stat.h>
#include <stdio.h>
#include <errno.h>

// stdio prototypes
int _getpid(void);
int _close(int file);
int _write(int file, char *ptr, int len);
int _read(int file, char *ptr, int len);
int _fstat(int file, struct stat *st);
int _lseek(int file, int ptr, int dir);
int _isatty(int file);
void *_sbrk(ptrdiff_t incr);
int kill(int pid, int sig);

volatile uint32_t systick = 0;

/* Set STM32 to 168 MHz using a 16 MHz HSE. */
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

    /* Enable peripheral clocks */
    rcc_periph_clock_enable(RCC_GPIOA);
    rcc_periph_clock_enable(RCC_GPIOB);
    rcc_periph_clock_enable(RCC_GPIOC);

    rcc_periph_clock_enable(RCC_USART1);
}

static void gpio_setup(void) {
    /* Set GPIO12-15 (in GPIO port D) to 'output push-pull'. */
    gpio_mode_setup(GPIOC, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO13);
    gpio_set_output_options(GPIOC, GPIO_OTYPE_OD, GPIO_OSPEED_2MHZ, GPIO13);

    /* Set PA9 and PA10 as AF for UART */
    gpio_mode_setup(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO9 | GPIO10);

    /* Set PA9 and PA10 to AF7 (usart1) */
    gpio_set_af(GPIOA, GPIO_AF7, GPIO9 | GPIO10);
}

static void usart_setup(void) {
    /* Setup UART parameters. */
    usart_set_baudrate(USART1, 2000000);
    usart_set_databits(USART1, 8);
    usart_set_stopbits(USART1, USART_STOPBITS_1);
    usart_set_parity(USART1, USART_PARITY_NONE);
    usart_set_flow_control(USART1, USART_FLOWCONTROL_NONE);
    usart_set_mode(USART1, USART_MODE_TX);

    /* Finally enable the USART. */
    usart_enable(USART1);
}

int main(void) {
    // Setup clocks and gpio
    clock_setup();
    gpio_setup();
    usart_setup();

    printf("\n\n\nStarting\n");

    // PC13 hi so LED is off by default
    gpio_set(GPIOC, GPIO13);

    // Timing for the superloop
    uint32_t now = 0, loop = 0, next_blink = 500, next_tick = 1000;

    /* Blink the LEDs (PD12, PD13, PD14 and PD15) on the board. */
    while (1) {
        now = systick;

        if (now >= next_blink) {
            gpio_toggle(GPIOC, GPIO13);
            next_blink = now + 500;
        }

        if (now >= next_tick) {
            printf("Tick %lu (loop = %lu)\n", now / 1000, loop);
            loop = 0;
            next_tick = now + 1000;
        }

        ++loop;
    }

    return 0;
}

// The sys_tick_handler should be called once every 1 ms.  Does nada
// except updating the systick variable
void sys_tick_handler(void) {
    ++systick;
};

// Send printf to uart1
int _write(int fd, char *ptr, int len) {
    int i = 0;

    /*
     * Write "len" of char from "ptr" to file id "fd"
     * Return number of char written.
     *
     * Only work for STDOUT, STDIN, and STDERR
     */
    if (fd > 2) {
        return -1;
    }
    while (*ptr && (i < len)) {
        usart_send_blocking(USART1, *ptr);
        if (*ptr == '\n') {
            usart_send_blocking(USART1, '\r');
        }
        i++;
        ptr++;
    }
    return i;
}

int _close(int file) {
    (void) file;
    return -1;
}

int _fstat(int file, struct stat *st) {
    (void) file;
    (void) st;
    return 0;
}

int _getpid(void) {
    return 1;
}

int _isatty(int file) {
    (void) file;
    return 1;
}

int kill(int pid, int sig) {
    (void) pid;
    (void) sig;
    errno = EINVAL;
    return -1;
}

int _lseek(int file, int ptr, int dir) {
    (void) file;
    (void) ptr;
    (void) dir;
    return 0;
}

int _read(int file, char *ptr, int len) {
    (void) file;
    (void) ptr;
    (void) len;
    return 0;
}

/* This is often required for malloc/dynamic memory */
void *_sbrk(ptrdiff_t incr) {
    extern char end; /* Defined by the linker script */
    static char *heap_end;
    char *prev_heap_end;

    if (heap_end == 0) {
        heap_end = &end;
    }

    prev_heap_end = heap_end;
    heap_end += incr;

    return (void *) prev_heap_end;
}
