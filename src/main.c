#include <avr/io.h>
#include <util/delay.h>

#define BLINK_DELAY_MS 100


void main(void) {
    DDRA |= _BV(PA0);
    DDRA |= _BV(PA2);
    DDRA |= _BV(PA4);

    while (1) {
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
}

