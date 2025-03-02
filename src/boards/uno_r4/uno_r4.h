#ifndef HUB75NANO_UNO_R4_H
#define HUB75NANO_UNO_R4_H

#include <Arduino.h>
#include "uno_r4_pin_helpers.h"

// actual pin numbers like in the arduino digitalwrite, can be changed here or in your project
#ifndef RA
#define RA A0 // row selector a
#endif
#ifndef RB
#define RB A1 // row selector b
#endif
#ifndef RC
#define RC A2 // row selector c
#endif
#ifndef RD
#define RD A3 // row selector d
#endif
// currently unused
#ifndef RE
// port3, but rightbit position (304!)
#define RE 8 // row selector e
#endif
#ifndef RF
#define RF 2 // red first byte
#endif
#ifndef GF
#define GF 3 // green first byte
#endif
#ifndef BF
#define BF 4 // blue first byte
#endif
#ifndef RS
#define RS 5 // red second byte
#endif
#ifndef GS
#define GS 6 // green second byte
#endif
#ifndef BS
#define BS 7 // blue second byte
#endif
#ifndef CLK
#define CLK 8 // clock signal
#endif
#ifndef LAT
#define LAT 10 // data latch
#endif
#ifndef OE
#define OE 9 // output enable
#endif

// helper definitions for setting/clearing
#define high_pin(pin) ((PORTS *)IO_PORT_START)->port[port_from_pin(arduino_pin_to_avr_pin(pin))].PCNTR3.POSR = 1 << bit_from_pin(arduino_pin_to_avr_pin(pin))
#define clear_pin(pin) ((PORTS *)IO_PORT_START)->port[port_from_pin(arduino_pin_to_avr_pin(pin))].PCNTR3.PORR = 1 << bit_from_pin(arduino_pin_to_avr_pin(pin))

// Set pin to output mode
#define set_pin_output(pin) pinMode(pin, OUTPUT);

#define HIGH_CLK high_pin(CLK)
#define CLEAR_CLK clear_pin(CLK)
#define HIGH_LAT high_pin(LAT)
#define CLEAR_LAT clear_pin(LAT)
#define HIGH_OE   \
    high_pin(OE); \
    __asm__ __volatile__("nop;")
#define CLEAR_OE \
    clear_pin(OE);
#define Clock                     \
    HIGH_CLK;                     \
    __asm__ __volatile__("nop;"); \
    CLEAR_CLK;                    \
    __asm__ __volatile__("nop;")
#define LATCH                     \
    HIGH_LAT;                     \
    __asm__ __volatile__("nop;"); \
    CLEAR_LAT;                    \
    __asm__ __volatile__("nop;")

// todo, no idea how
#define OVERFLOW 0

#endif // HUB75NANO_UNO_R4_H
