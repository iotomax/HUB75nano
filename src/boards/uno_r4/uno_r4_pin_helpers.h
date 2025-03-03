#ifndef HUB75NANO_UNO_R4_PIN_HELPER_H
#define HUB75NANO_UNO_R4_PIN_HELPER_H

// ------------------------------------------------------------------------
// Physical pin number definitions for the RA4M1 chip used on UNO R4 WiFi
// (These definitions are similar to those in the Minima code. They assume the
//  same RA4M1 chip is used, though the board layout is different.)
// ------------------------------------------------------------------------
#define P000 0
#define P001 1
#define P002 2
#define P003 3
#define P004 4
#define P005 5
#define P006 6
#define P007 7
#define P008 8
#define P010 10
#define P011 11
#define P012 12
#define P013 13
#define P014 14
#define P015 15

#define P100 100
#define P101 101
#define P102 102
#define P103 103
#define P104 104
#define P105 105  // <--- Digital Pin 2 (R1) on UNO R4 WiFi
#define P106 106  // <--- Digital Pin 6 (G2)
#define P107 107  // <--- Digital Pin 7 (B2)
#define P108 108
#define P109 109  // <--- Digital Pin 3 (G1) uses P104 in our mapping below; see below.
#define P110 110  // <--- Digital Pin 4 (B1) uses P103 in our mapping below.
#define P111 111
#define P112 112  // <--- Digital Pin 10 (LAT)
#define P113 113
#define P114 114
#define P115 115

#define P201 201
#define P202 202
#define P203 203
#define P204 204
#define P205 205
#define P206 206
#define P212 212
#define P213 213

#define P300 300
#define P301 301  // D0 (Serial RX)
#define P302 302  // D1 (Serial TX)
#define P303 303  // <--- Digital Pin 9 (OE)
#define P304 304  // <--- Digital Pin 8 (CLK)
#define P305 305
#define P306 306
#define P307 307

// ------------------------------------------------------------------------
// Error definitions
#define NOT_A_PORT 255
#define NOT_A_PIN 255

// ------------------------------------------------------------------------
// IO_PORT_START is the base address for the RA4M1 port registers
#define IO_PORT_START 0x40040000

// Offsets for each port (assumed 0 through 9)
#define PORT0_OFFSET 0
#define PORT1_OFFSET 1
#define PORT2_OFFSET 2
#define PORT3_OFFSET 3
#define PORT4_OFFSET 4
#define PORT5_OFFSET 5
#define PORT6_OFFSET 6
#define PORT7_OFFSET 7
#define PORT8_OFFSET 8
#define PORT9_OFFSET 9

// Return the port offset for a given physical pin number.
constexpr uint8_t port_from_pin(uint16_t pin) {
  return (uint8_t)(pin < 100    ? PORT0_OFFSET
                   : pin < 200  ? PORT1_OFFSET
                   : pin < 300  ? PORT2_OFFSET
                   : pin < 400  ? PORT3_OFFSET
                   : pin < 500  ? PORT4_OFFSET
                   : pin < 600  ? PORT5_OFFSET
                   : pin < 700  ? PORT6_OFFSET
                   : pin < 800  ? PORT7_OFFSET
                   : pin < 900  ? PORT8_OFFSET
                   : pin < 1000 ? PORT9_OFFSET
                                : NOT_A_PORT);
}

// Return the bit position within the port for a given physical pin.
constexpr uint8_t bit_from_pin(uint8_t pin) {
  return (uint8_t)(pin < 100    ? pin
                   : pin < 200  ? pin - 100
                   : pin < 300  ? pin - 200
                   : pin < 400  ? pin - 300
                   : pin < 500  ? pin - 400
                   : pin < 600  ? pin - 500
                   : pin < 700  ? pin - 600
                   : pin < 800  ? pin - 700
                   : pin < 900  ? pin - 800
                   : pin < 1000 ? pin - 900
                                : NOT_A_PIN);
}

// ------------------------------------------------------------------------
// Map Arduino "logical" pins to the RA4M1’s physical pins for the UNO R4 WiFi.
// These mappings have been adjusted based on the ABX00087 full pinout and schematics.
// For our configuration:
//   D0  → P301 (Serial RX)
//   D1  → P302 (Serial TX)
//   D2  → P105 (R1)
//   D3  → P104 (G1)
//   D4  → P103 (B1)
//   D5  → P102 (R2)
//   D6  → P106 (G2)
//   D7  → P107 (B2)
//   D8  → P304 (CLK)
//   D9  → P303 (OE)
//   D10 → P112 (LAT)
//   A0  → P000 (Row A)
//   A1  → P001 (Row B)
//   A2  → P002 (Row C)
//   A3  → P003 (Row D)
constexpr uint8_t arduino_pin_to_avr_pin(uint8_t pin) {
  return (uint8_t)(
      (pin == 0)   ? P301 :   // D0
      (pin == 1)   ? P302 :   // D1
      (pin == 2)   ? P105 :   // D2: R1
      (pin == 3)   ? P104 :   // D3: G1
      (pin == 4)   ? P103 :   // D4: B1
      (pin == 5)   ? P102 :   // D5: R2
      (pin == 6)   ? P106 :   // D6: G2
      (pin == 7)   ? P107 :   // D7: B2
      (pin == 8)   ? P304 :   // D8: CLK
      (pin == 9)   ? P303 :   // D9: OE
      (pin == 10)  ? P112 :   // D10: LAT
      (pin == A0)  ? P000 :   // A0: Row A
      (pin == A1)  ? P001 :   // A1: Row B
      (pin == A2)  ? P002 :   // A2: Row C
      (pin == A3)  ? P003 :   // A3: Row D
      NOT_A_PIN);
}

#endif // HUB75NANO_UNO_R4_PIN_HELPER_H
