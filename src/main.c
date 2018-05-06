#include <stdio.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <util/delay.h>
#include "hmi.h"
#include "print.h"
#include "../lib/hd44780_111/hd44780.h"
#include "../lib/helius_microrl/microrl.h"
#include "../lib/andygock_avr_uart/uart.h"
#include "../lib/matejx_avr_lib/mfrc522.h"
#include <time.h>
#include <stdlib.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "cli_microrl.h"

#define UART_BAUD           9600
#define UART_STATUS_MASK    0x00FF

#define LED_RED             PORTA0
#define LED_GREEN           PORTA2
#define LED_BLUE            PORTA4

void door(void);

microrl_t rl;
microrl_t *prl = &rl;

static inline void init_leds(void) {
    DDRA |= _BV(LED_RED) | _BV(LED_GREEN) | _BV(LED_BLUE);
    PORTA &= ~(_BV(LED_RED) | _BV(LED_GREEN) | _BV(LED_BLUE));
    DDRB |= _BV(DDB7);
    PORTB &= ~_BV(PORTB7);
}


static inline void init_microrl(void) {
    microrl_init (prl, uart0_puts);
    microrl_set_execute_callback (prl, cli_execute);
}


static inline void printversion(void) {
    uart1_puts_p(PSTR(VER_FW));
    uart1_puts_p(PSTR(VER_LIBC));
}


static inline void init_timer(void) {
    TCCR1A = 0;
    TCCR1B = 0;
    TCCR1B |= _BV(WGM12);
    TCCR1B |= _BV(CS12);
    OCR1A = 62549;
    TIMSK1 |= _BV(OCIE1A);
}

static inline void init_uart1(void) {
    uart1_init(UART_BAUD_SELECT(UART_BAUD, F_CPU));
    uart1_puts_p(VER_FW);
    uart1_puts_p(VER_LIBC);
}

static inline void init_uart0(void) {
    uart0_init(UART_BAUD_SELECT(UART_BAUD, F_CPU));
    uart0_puts_p(PSTR("Console started\r\n"));
    uart0_puts_p(stud_name);
    uart0_puts_p(PSTR("\r\n"));
    uart0_puts_p(PSTR("Type help for available commands\r\n"));
}

static inline void init_rfid(void) {
    MFRC522_init();
    PCD_Init();
}

void main(void) {
    init_leds();
    lcd_init();
    lcd_clrscr();
    lcd_puts_P(stud_name);
    init_uart1();
    init_uart0();
    init_rfid();
    init_microrl();
    init_timer();
    printversion();

    while (1) {
        door();
        microrl_insert_char(prl, (uart0_getc() & UART_STATUS_MASK));
    }
}
ISR(TIMER1_COMPA_vect) {
    system_tick();
}
