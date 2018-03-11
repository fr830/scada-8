#include <stdio.h>
#define __ASSERT_USE_STDERR
#include <assert.h>
#include <avr/io.h>
#include <util/delay.h>
#include "uart.h"

#define BLINK_DELAY_MS 100

static inline void init_leds() {
    DDRA |= _BV(PA0);
    DDRA |= _BV(PA2);
    DDRA |= _BV(PA4);
    DDRB |= _BV(DDB7);
    PORTB &= ~_BV(PORTB7);
}

static inline void blink_leds(void) {
    PORTA |= _BV(PA4);
    _delay_ms(BLINK_DELAY_MS);
    PORTA &= ~_BV(PA4);
    _delay_ms(BLINK_DELAY_MS);
    PORTA |= _BV(PA2);
    _delay_ms(BLINK_DELAY_MS);
    PORTA &= ~_BV(PA2);
    _delay_ms(BLINK_DELAY_MS);
    PORTA |= _BV(PA0);
    _delay_ms(BLINK_DELAY_MS);
    PORTA &= ~_BV(PA0);
    _delay_ms(BLINK_DELAY_MS);
}

static inline void init_errcon(void) {
    simple_uart1_init();
    stderr = &simple_uart1_out;
    fprintf(stderr,
            "Version: %s built on: %s %s\navr-libc version: %s avr-gcc version: %s\n",
            FW_VERSION, __DATE__, __TIME__, __AVR_LIBC_VERSION_STRING__, __VERSION__);
}


void main(void) {
    init_leds();
    init_errcon();
    /* Test assert - REMOVE IN FUTURE LABS */
    char *array;
    uint32_t i = 1;
    extern int __heap_start, *__brkval;
    int v;
    /* As sizeof(char) is guaranteed to be 1 it is left here only for better
     * understanding how malloc works. No need to add it in production.
     */
    array = malloc( i * sizeof(char));
    assert(array != NULL);
    /* End test assert */

    while (1) {
        blink_leds();
        /* Test assert - REMOVE IN FUTURE LABS */
        /*
         * Increase memory allocated for array by 100 chars
         * until we have eaten it all and print space between stack and heap.
         * That is how assert works in run-time.
         */
        array = realloc( array, (i++ * 100) * sizeof(char));
        fprintf(stderr, "%d\n",
                (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval));
        assert(array != NULL);
        /* End test assert */
    }
}

