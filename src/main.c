#include <avr/io.h>
#include <util/delay.h>

#define BLINK_DELAY_MS 100


void main(void) {
    /* Set port B pin 7 for output for Arduino Mega yellow LED */
    DDRB |= _BV(DDB7);

    while (1) {
        //TODO Student can use bitwise XOR and toggle a bit if such technique is known already.
        /* Set port B pin 7 high to turn Arduino Mega yellow LED on */
        PORTB |= _BV(PORTB7);
        _delay_ms(BLINK_DELAY_MS);
        /* Set port B pin 7 high to turn Arduino Mega yellow LED off */
        PORTB &= ~_BV(PORTB7);
        _delay_ms(BLINK_DELAY_MS);
    }
}
