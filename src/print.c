#include <stdio.h>
#include "../lib/andygock_avr_uart/uart.h"
#include <avr/pgmspace.h>
#include <string.h>

void print_ascii_tbl(void) {
    for (char c = ' '; c <= '~'; c++) {
        uart0_putc(c);
    }

    uart0_puts_p(PSTR("\r\n"));
}

void print_for_human (const unsigned char *array, const size_t len) {
    for (size_t i = 0; i < len; i++) {
        if (array[i] < 32 || array[i] > 126) {
            uart0_puts_p(PSTR("\"0x"));
            uart0_putc((array[i] >> 4) + ((array[i] >> 4) <= 9 ? 0x30 : 0x37));
            uart0_putc((array[i] & 0x0F) + ((array[i] & 0x0F) <= 9 ? 0x30 : 0x37));
            uart0_putc('"');
        } else {
            uart0_putc(array[i]);
        }
    }

    uart0_puts_p(PSTR("\r\n"));
}

void print_hex(const uint8_t b) {
    uart0_putc((b >> 4) + ((b >> 4) <= 9 ? 0x30 : 0x37));
    uart0_putc((b & 0x0F) + ((b & 0x0F) <= 9 ? 0x30 : 0x37));
}

// Based on https://stackoverflow.com/a/23898449/266720
void tallymarker_hextobin(const char * str, uint8_t * bytes, size_t blen) {
    uint8_t  pos;
    uint8_t  idx0;
    uint8_t  idx1;
    // mapping of ASCII characters to hex values
    const uint8_t hashmap[] = {
        0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, // 01234567
        0x08, 0x09, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // 89:;<=>?
        0x00, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x00, // @ABCDEFG
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // HIJKLMNO
    };
    memset(bytes, 0, blen);

    for (pos = 0; ((pos < (blen * 2)) && (pos < strlen(str))); pos += 2) {
        idx0 = ((uint8_t)str[pos + 0] & 0x1F) ^ 0x10;
        idx1 = ((uint8_t)str[pos + 1] & 0x1F) ^ 0x10;
        bytes[pos / 2] = (uint8_t)(hashmap[idx0] << 4) | hashmap[idx1];
    };
}