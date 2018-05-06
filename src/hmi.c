#include <avr/pgmspace.h>
#include "hmi.h"

const char stud_name[] PROGMEM = "Mikus Secret";
const char m1[] PROGMEM = "January";
const char m2[] PROGMEM = "February";
const char m3[] PROGMEM = "March";
const char m4[] PROGMEM = "April";
const char m5[] PROGMEM = "May";
const char m6[] PROGMEM = "June";

const char* const name_month[] PROGMEM = {
    m1,
    m2,
    m3,
    m4,
    m5,
    m6
};