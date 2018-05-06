# Arduino Mega RFID-RC522 wiring

## Introduction

**NB! RFID-RC522 requires only 3.3 V DC input voltage. Never connect it to 5V input.**

Wiring is valid for Velleman RFID reader module and Velleman LCD shield.

## Wiring illustration

![arduino-mega-rfid-rc522-wiring-for-velleman-boards.png](arduino-mega-rfid-rc522-wiring-for-velleman-boards.png)

<div class=pagebreak></div>

## Wiring table

| Signal | ATMega2560 port and pin | Arduino Mega 2560 pin | LCD1602 keypad shield | RFID-RC522 | Wire colour in illustration |
| --- | --- | --- | --- | --- |  --- |
| Slave select | PORTB 0 | 53 | - | NSS | White |
| SPI clock | PORTB 1 | 52 | - | SCK | Orange |
| Master out slave in | PORTB 2 | 51 | - | MOSI | Green |
| Master in slave out | PORTB 3 | 50 | - | MISO | Yellow |
| RF522 reset | PORTL 0 | 49 | - | RST | Brown |
| Ground | GND | GND | GND | GND | Black |
| 3,3 V DC | - | 3V3 | 3V3 | VCC | Red |

<div class=pagebreak></div>

## Wiring photo

![arduino-mega-rfid-rc522-wiring-for-velleman-boards-photo.jpg](arduino-mega-rfid-rc522-wiring-for-velleman-boards-photo.jpg)
