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

void main(void) {
    while (1) {
        blink_leds();
    }
}

