#ifndef HUB75NANO_BOARD_H
#define HUB75NANO_BOARD_H

#include <Arduino.h>

// check we are on uno or nano, get pinout and how we access them
#ifndef ARDUINO_AVR_NANO
#ifndef ARDUINO_AVR_UNO
#error "This library only supports the Arduino nano and Uno, so the atm368p with 2kb sram, 1kb eeprom and 32kb flash. For other chips/boards, please see the internet or try to adapt this library here, but no guarantees"
#else
#include "boards/uno/uno.h"
#endif
#else
#include "boards/nano/nano.h"
#endif

#ifndef PANEL_X
#error "Panel size (x) has to be defined!"
#endif

#ifndef PANEL_Y
#error "Panel size (y) has to be defined!"
#endif

#endif // HUB75NANO_BOARD_H