#include <stdio.h>
#define __ASSERT_USE_STDERR
#include <assert.h>
#include <avr/io.h>
#include <util/delay.h>
#include "uart.h"
#include <avr/pgmspace.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "hmi.h"
#include "../lib/hd44780_111/hd44780.h"


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

static inline void init_ioconn(void) {
    simple_uart0_init();
    stdin = stdout = &simple_uart0_io;
    fprintf_P(stdout, stud_name);
    fprintf(stdout, "\n");
    fprintf_P(stdout, PSTR("Console started\n"));
}

void print_ascii_tbl(FILE *stream) {
    for (char c = ' '; c <= '~'; c++) {
        fprintf(stream, "%c ", c);
    }

    fprintf(stream, "\n");
}

void print_for_human (FILE *stream, const unsigned char *array,
                      const size_t len) {
    for (size_t i = 0; i < len; i++) {
        char c = array[i];

        if (c >= ' ' && c <= '~') {
            fprintf(stream, "%c", c);
        } else {
            fprintf(stream, "\"0x%02X\"", c);
        }
    }

    fprintf(stream, "\n");
}

void main(void) {
    init_leds();
    init_errcon();
    init_ioconn();
    lcd_init();
    lcd_clrscr();
    print_ascii_tbl(stdout);
    unsigned char ascii[128] = {0};

    for (unsigned char i = 0; i < 128; i++) {
        ascii[i] = i;
    }

    print_for_human(stdout, ascii, 128);
    lcd_puts_P(stud_name);
    char m;

    while (1) {
        blink_leds();
        fprintf_P(stdout, PSTR("Enter Month name first letter >"));
        m = toupper(getchar());
        fprintf(stdout, "%c\n", m);
        lcd_clr(LCD_ROW_2_START, LCD_VISIBLE_COLS);
        lcd_goto(LCD_ROW_2_START);

        for (uint8_t i = 0; i < 6; i++) {
            if (!strncmp_P(&m, (PGM_P)pgm_read_word(&(name_month[i])), 1)) {
                fprintf_P(stdout, (PGM_P)pgm_read_word(&(name_month[i])));
                fprintf(stdout, "\n");
                lcd_puts_P((PGM_P)pgm_read_word(&(name_month[i])));
                lcd_puts(" ");
            }
        }
    }
}
