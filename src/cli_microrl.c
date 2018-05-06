#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <avr/pgmspace.h>
#include "../lib/hd44780_111/hd44780.h"
#include "../lib/andygock_avr_uart/uart.h"
#include "../lib/helius_microrl/microrl.h"
#include "hmi.h"
#include "cli_microrl.h"
#include <time.h>
#include "print.h"
#include "../lib/andy_brown_memdebug/memdebug.h"
#include "../lib/matejx_avr_lib/mfrc522.h"

#define NUM_ELEMS(x)        (sizeof(x) / sizeof((x)[0]))
#define LED_RED PORTA0

char * current_display;
char * previous_display;
door_state_t door_state = door_closed;
display_state_t display_state = display_no_updates;
time_t time_y2k_cpy;
time_t door_open_time;
time_t msg_display_time;
time_t card_read_time;
time_t now_time;

const char help_cmd[] PROGMEM = "help";
const char help_help[] PROGMEM = "Print available commands";
const char example_cmd[] PROGMEM = "example";
const char example_help[] PROGMEM =
    "Prints provided 3 arguments Usage: example <argument> <argument> <argument>";
const char ver_cmd[] PROGMEM = "version";
const char ver_help[] PROGMEM = "Print firmware version";
const char ascii_cmd[] PROGMEM = "ascii";
const char ascii_help[] PROGMEM = "Print ASCII table";
const char month_cmd[] PROGMEM = "month";
const char month_help[] PROGMEM =
    "Print and display matching month Usage: month <starting letter>";
const char mem_cmd[] PROGMEM = "mem";
const char mem_help[] PROGMEM = "Print memory usage";
const char read_cmd[] PROGMEM = "read";
const char read_help[] PROGMEM = "Read card and print cards UID";
const char add_cmd[] PROGMEM = "add";
const char add_help[] PROGMEM = "Add UID to list. Usage: add <uid> <name>";
const char print_cmd[] PROGMEM = "print";
const char print_help[] PROGMEM = "Print existing list";
const char rm_cmd[] PROGMEM = "rm";
const char rm_help[] PROGMEM = "Remove UID from list. Usage: rm <uid>";

const cli_cmd_t cli_cmds[] = {
    {help_cmd, help_help, cli_print_help, 0},
    {ver_cmd, ver_help, cli_print_ver, 0},
    {example_cmd, example_help, cli_example, 3},
    {ascii_cmd, ascii_help, cli_print_ascii_tbl, 0},
    {month_cmd, month_help, cli_month, 1},
    {mem_cmd, mem_help, cli_mem, 0},
    {read_cmd, read_help, cli_rfid_read, 0},
    {add_cmd, add_help, cli_rfid_add, 2},
    {rm_cmd, rm_help, cli_remove_card, 1},
    {print_cmd, print_help, cli_rfid_print, 0}
};

typedef struct card {
    uint8_t *uid;
    size_t size;
    char *name;
    struct card *next;
} card;

card *card_ptr = NULL;
int exists = 0;
int same = 0;


void cli_remove_card(const char *const *argv) {
    size_t rm_uid_size = strlen(argv[1]) / 2;
    uint8_t *rm_uid = (uint8_t *)malloc(rm_uid_size * sizeof(uint8_t));
    tallymarker_hextobin(argv[1], rm_uid, rm_uid_size);
    card *current = card_ptr;
    card *last = NULL;

    while (current != NULL) {
        if (current->size == rm_uid_size) {
            same = 1;

            for (size_t i = 0; i < rm_uid_size; i++) {
                if (current->uid[i] != rm_uid[i]) {
                    same = 0;
                    break;
                }
            }

            if (same) {
                if (last == NULL) {
                    card_ptr = current->next;
                    uart0_puts_p(PSTR("Removed card UID: "));

                    for (size_t i = 0; i < rm_uid_size; i++) {
                        print_hex(rm_uid[i]);
                    }

                    uart0_puts_p(PSTR("\r\n"));
                    free(rm_uid);
                    free(current->uid);
                    free(current->name);
                    free(current);
                    return;
                } else {
                    last->next = current->next;
                    uart0_puts_p(PSTR("Removed card UID: "));

                    for (size_t i = 0; i < rm_uid_size; i++) {
                        print_hex(rm_uid[i]);
                    }

                    uart0_puts_p(PSTR("\r\n"));
                    free(rm_uid);
                    free(current->uid);
                    free(current->name);
                    free(current);
                    return;
                }
            }
        }

        last = current;
        current = current->next;
    }

    uart0_puts_p(PSTR("This UID is not in the list\r\n"));
}


void cli_rfid_add(const char *const *argv) {
    (void)argv;
    card *new_card;
    new_card = (card *)malloc(sizeof(card));

    if (new_card == NULL) {
        uart0_puts_p(PSTR("Failed to allocate memory\r\n"));
        return;
    }

    if (card_ptr->uid == NULL) {
        uart0_puts_p(PSTR("Failed to allocate memory\r\n"));
        free(card_ptr);
        return;
    }

    new_card = malloc(sizeof(card));
    new_card->size = strlen(argv[1]) / 2;
    new_card->uid = (uint8_t *)malloc(new_card->size * sizeof(uint8_t));
    new_card->name = malloc(sizeof(char) * strlen(argv[2]));
    tallymarker_hextobin(argv[1], new_card->uid, new_card->size);
    strcpy(new_card->name, argv[2]);

    if (new_card->size > 10) {
        uart0_puts_p(PSTR("UID is bigger than 10 bytes\r\n"));
        return;
    }

    card *c = card_ptr;

    while (c != NULL) {
        if (c->size == new_card->size) {
            same = 1;

            for (size_t i = 0; i < c->size; i++) {
                if (new_card->uid[i] != c->uid[i]) {
                    same = 0;
                }
            }

            if (same) {
                uart0_puts_p(PSTR("UID is already in the list\r\n"));
                return;
            }
        }

        c = c->next;
    }

    new_card->next = card_ptr;
    card_ptr = new_card;
    uart0_puts_p(PSTR("Added card with UID: "));

    for (size_t i = 0; i < card_ptr->size; i++) {
        print_hex(card_ptr->uid[i]);
    }

    uart0_puts_p(PSTR(" Size: "));
    print_hex(card_ptr->size);
    uart0_puts_p(PSTR(" Holder name: "));
    uart0_puts(card_ptr->name);
    uart0_puts_p(PSTR("\r\n"));
}


void cli_rfid_print(const char *const *argv) {
    (void) argv;
    char buffer[20];
    int count = 1;
    card *current = card_ptr;

    if (current == NULL) {
        uart0_puts_p(PSTR("No UIDs in the list\r\n"));
    }

    while (current != NULL) {
        sprintf_P(buffer, PSTR("%u. "), count);
        uart0_puts(buffer);
        uart0_puts_p(PSTR(" UID: "));

        for (size_t i = 0; i < current->size; i++) {
            print_hex(current->uid[i]);
        }

        uart0_puts_p(PSTR(" Name: "));
        uart0_puts(current->name);
        uart0_puts_p(PSTR("\r\n"));
        current = current->next;
        count++;
    }
}


void cli_print_help(const char *const *argv) {
    (void) argv;
    uart0_puts_p(PSTR("Implemented commands:\r\n"));

    for (uint8_t i = 0; i < NUM_ELEMS(cli_cmds); i++) {
        uart0_puts_p(cli_cmds[i].cmd);
        uart0_puts_p(PSTR(" : "));
        uart0_puts_p(cli_cmds[i].help);
        uart0_puts_p(PSTR("\r\n"));
    }
}


void cli_example(const char *const *argv) {
    uart0_puts_p(PSTR("Arguments:\r\n"));

    for (uint8_t i = 1; i < 4; i++) {
        uart0_puts(argv[i]);
        uart0_puts_p(PSTR("\r\n"));
    }
}


void cli_print_ver(const char *const *argv) {
    (void) argv;
    uart0_puts_p(PSTR(VER_FW));
    uart0_puts_p(PSTR(VER_LIBC));
}


void cli_print_ascii_tbl(const char *const *argv) {
    (void) argv;
    print_ascii_tbl();
    unsigned char ascii [128] = {0};

    for (unsigned char i = 0; i < 128; i++) {
        ascii[i] = i;
    }

    print_for_human(ascii, 128);
}


void cli_month(const char *const *argv) {
    lcd_clr(LCD_ROW_2_START, LCD_VISIBLE_COLS);
    lcd_goto(LCD_ROW_2_START);

    for (uint8_t i = 0; i < 6; i++) {
        if (!strncmp_P(argv[1], (PGM_P)pgm_read_word(&(name_month[i])),
                       strlen(argv[1]))) {
            uart0_puts_p((PGM_P)pgm_read_word(&(name_month[i])));
            uart0_puts_p(PSTR("\r\n"));
            lcd_puts_P((PGM_P)pgm_read_word(&(name_month[i])));
            lcd_puts(" ");
        }
    }
}


void cli_print_cmd_error(void) {
    uart0_puts_p(PSTR("Command not found\r\nType <help> to get help.\r\n"));
}


void cli_print_cmd_arg_error(void) {
    uart0_puts_p(
        PSTR("Incorrect parameters\r\nType <help>\r\n"));
}


void cli_mem(const char *const *argv) {
    (void) argv;
    char print_buf[256] = {0x00};
    uint16_t space = getFreeMemory();
    static uint16_t prev_space;
    uart0_puts_p(PSTR("\r\nSpace between stack and heap:\r\n"));
    uart0_puts_p(PSTR("Current: "));
    uart0_puts(itoa(space, print_buf, 10));
    uart0_puts_p(PSTR("\r\nPrevious: "));
    uart0_puts(itoa(prev_space, print_buf, 10));
    uart0_puts_p(PSTR("\r\nChange: "));
    uart0_puts(itoa((space - prev_space), print_buf, 10));
    uart0_puts_p(PSTR("\r\n\r\nHeap statistics\r\n"));
    uart0_puts_p(PSTR("Used: "));
    uart0_puts(itoa(getMemoryUsed(), print_buf, 10));
    uart0_puts_p(PSTR("\r\nFree: "));
    uart0_puts(itoa(getFreeMemory(), print_buf, 10));
    uart0_puts_p(PSTR("\r\nLargest non freelist block: "));
    uart0_puts(itoa(getLargestNonFreeListBlock(), print_buf, 10));
    uart0_puts_p(PSTR("\r\nLargest dynamically allocable block: "));
    uart0_puts(itoa(getLargestAvailableMemoryBlock(), print_buf, 10));
    uart0_puts_p(PSTR("\r\n\r\nFreelist\r\n"));
    uart0_puts_p(PSTR("Freelist size: "));
    uart0_puts(itoa(getFreeListSize(), print_buf, 10));
    uart0_puts_p(PSTR("\r\nBlocks in freelist: "));
    uart0_puts(itoa(getNumberOfBlocksInFreeList(), print_buf, 10));
    uart0_puts_p(PSTR("\r\nLargest block in freelist: "));
    uart0_puts(itoa(getLargestBlockInFreeList(), print_buf, 10));
    uart0_puts_p(PSTR("\r\n\r\n"));
    prev_space = space;
}


void cli_rfid_read(const char *const *argv) {
    (void) argv;
    Uid uid;
    Uid *uid_ptr = &uid;
    byte bufferATQA[10];
    byte bufferSize[10];
    uart1_puts_p(PSTR("\r\n"));

    if (PICC_IsNewCardPresent()) {
        uart0_puts_p(PSTR("Card selected\r\n"));
        PICC_ReadCardSerial(uid_ptr);
        uart0_puts_p(PSTR("Card type: "));
        uart0_puts(PICC_GetTypeName(PICC_GetType(uid.sak)));
        uart0_puts_p(PSTR("\r\n"));
        uart0_puts_p(PSTR("Card UID: "));

        for (byte i = 0; i < uid.size; i++) {
            print_hex(uid.uidByte[i]);
        }

        uart0_puts_p(PSTR(" (size: "));
        print_hex(uid.size);
        uart0_puts_p(PSTR(" bytes)"));
        PICC_WakeupA(bufferATQA, bufferSize);
        uart0_puts_p(PSTR("\r\n"));
    } else {
        uart0_puts_p((PSTR("Unable to select card.\r\n")));
    }
}


int cli_execute(int argc, const char *const *argv) {
    uart0_puts_p(PSTR("\r\n"));

    for (uint8_t i = 0; i < NUM_ELEMS(cli_cmds); i++) {
        if (!strcmp_P(argv[0], cli_cmds[i].cmd)) {
            if ((argc - 1) != cli_cmds[i].func_argc) {
                cli_print_cmd_arg_error();
                return 0;
            }

            cli_cmds[i].func_p (argv);
            return 0;
        }
    }

    cli_print_cmd_error();
    return 0;
}


void door(void) {
    Uid uid;
    Uid *uid_ptr = &uid;
    char lcd_buf[16] = {0x00};

    if (PICC_IsNewCardPresent()) {
        PICC_ReadCardSerial(uid_ptr);
        card *current = card_ptr;

        if (current == NULL) {
            door_state = door_closed;
            display_state = display_access_denied;
        }

        while (current != NULL) {
            if (memcmp(uid.uidByte, current->uid, uid.size) == 0) {
                door_state = door_opening;

                if (exists == 1) {
                    display_state = display_name;
                } else {
                    display_state = display_no_updates;
                }

                exists = 1;
                card_read_time = time(0);

                if (strcmp(current_display, current->name) != 0) {
                    display_state = display_name;
                    current_display = current->name;
                    break;
                } else {
                    return;
                }
            } else {
                exists = 0;
            }

            current = current->next;
        }

        if (exists == 0) {
            door_state = door_closing;
            display_state = display_access_denied;
        }
    }

    now_time = time(NULL);

    if (((now_time - card_read_time) > 3) && (exists == 1)) {
        current_display = NULL;
        display_state = display_clear;
    }

    switch (door_state) {
    case door_opening:
        door_open_time = time(NULL);
        door_state = door_open;
        break;

    case door_open:
        time_y2k_cpy = time(NULL);

        if ((time_y2k_cpy - door_open_time) > 2) {
            door_state = door_closing;
        }

        break;

    case door_closing:
        door_state = door_closed;
        break;

    case door_closed:
        PORTA &= ~_BV(LED_RED);
        break;
    }

    switch (display_state) {
    case display_name:
        if (previous_display_state != display_name) {
            PORTA ^= _BV(LED_RED);
        }

        if ((strcmp(previous_display, current_display) == 0) &&
                (previous_display_state == display_name)) {
            display_state = display_clear;
            previous_display_state = display_name;
            break;
        }

        lcd_clr(LCD_ROW_2_START, LCD_VISIBLE_COLS);
        lcd_goto(LCD_ROW_2_START);

        if (current_display != NULL) {
            strncpy(lcd_buf, current_display, LCD_VISIBLE_COLS);
            lcd_puts(lcd_buf);
            previous_display = current_display;
        } else {
            lcd_puts_P(PSTR("Read error"));
        }

        msg_display_time = time(NULL);
        display_state = display_clear;
        previous_display_state = display_name;
        break;

    case display_access_denied:
        if (previous_display_state == display_access_denied) {
            display_state = display_clear;
            break;
        }

        lcd_clr(LCD_ROW_2_START, LCD_VISIBLE_COLS);
        lcd_goto(LCD_ROW_2_START);
        lcd_puts_P(PSTR("Access denied"));
        msg_display_time = time(NULL);
        display_state = display_clear;
        previous_display_state = display_access_denied;
        break;

    case display_clear:
        time_y2k_cpy = time(NULL);

        if ((time_y2k_cpy - msg_display_time) > 3) {
            lcd_clr(LCD_ROW_2_START, LCD_VISIBLE_COLS);
            display_state = display_no_updates;
            previous_display_state = display_clear;
        }

        break;

    case display_no_updates:
        break;
    }
}
