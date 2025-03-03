#ifndef HUB75NANO_UNO_R4_H
#define HUB75NANO_UNO_R4_H

#include <Arduino.h>
#include "uno_r4_pin_helpers.h"

// ------------------------------------------------------------------------
// Pin definitions for the external 32×64 HUB75 panel on the UNO R4 WiFi
// (ABX00087). 
//
// IMPORTANT: This mapping assumes you have disabled the onboard 12×8 LED matrix,
// freeing D2–D13 for your HUB75 signals.
// ------------------------------------------------------------------------

// Color data signals
#define R1 2   // Top half red
#define G1 3   // Top half green
#define B1 4   // Top half blue
#define R2 5   // Bottom half red
#define G2 6   // Bottom half green
#define B2 7   // Bottom half blue

// Control signals
#define CLK 8  // Clock signal
#define OE  9  // Output enable
#define LAT 10 // Latch

// Row address signals (for up to 16 rows)
// Using analog pins A0-A3 (which become D14-D17)
#define A A0  // Row address bit 0
#define B A1  // Row address bit 1
#define C A2  // Row address bit 2
#define D A3  // Row address bit 3

// For compatibility with legacy naming (if code refers to RF/GF/BF and RS/GS/BS)
#define RF R1
#define GF G1
#define BF B1
#define RS R2
#define GS G2
#define BS B2

// ------------------------------------------------------------------------
// Helper macros for fast direct port writes (using the RA4M1’s PORTS structure)
// ------------------------------------------------------------------------
#define high_pin(pin) ((PORTS *)IO_PORT_START)->port[ port_from_pin(arduino_pin_to_avr_pin(pin)) ].PCNTR3.POSR = (1 << bit_from_pin(arduino_pin_to_avr_pin(pin)))
#define clear_pin(pin) ((PORTS *)IO_PORT_START)->port[ port_from_pin(arduino_pin_to_avr_pin(pin)) ].PCNTR3.PORR = (1 << bit_from_pin(arduino_pin_to_avr_pin(pin)))

// Set pin to output mode (for clarity)
#define set_pin_output(pin) pinMode(pin, OUTPUT)

// Basic control macros for CLK, LAT, and OE (for inline toggling)
#define HIGH_CLK high_pin(CLK)
#define CLEAR_CLK clear_pin(CLK)
#define HIGH_LAT high_pin(LAT)
#define CLEAR_LAT clear_pin(LAT)
#define HIGH_OE high_pin(OE)
#define CLEAR_OE clear_pin(OE)

// Inline macros to toggle CLK and LAT with a very short delay using NOPs
#define Clock() do { HIGH_CLK; __asm__ __volatile__("nop;"); CLEAR_CLK; __asm__ __volatile__("nop;"); } while(0)
#define Latch() do { HIGH_LAT; __asm__ __volatile__("nop;"); CLEAR_LAT; __asm__ __volatile__("nop;"); } while(0)

#endif // HUB75NANO_UNO_R4_H
