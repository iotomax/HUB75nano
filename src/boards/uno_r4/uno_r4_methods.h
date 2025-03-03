#ifndef HUB75NANO_UNO_R4_METHODS_H
#define HUB75NANO_UNO_R4_METHODS_H

#include "uno_r4.h"
#include "uno_r4_pin_helpers.h"
// Optionally include additional helper headers or project settings
#include "../method_helper.h"
#include "../../Settings.h"

// ------------------------------------------------------------------------
// _set_color
// Sets all six color lines in one call using a 6‑bit value:
//   Bits 0–2: Top half (R1, G1, B1)
//   Bits 3–5: Bottom half (R2, G2, B2)
// ------------------------------------------------------------------------
#ifdef PANEL_MAX_SPEED
__attribute__((always_inline))
#endif
inline void _set_color(uint8_t value) {
  // For maximum speed, this uses direct register writes.
  // Top half:
  ((PORTS *)IO_PORT_START)->port[port_from_pin(arduino_pin_to_avr_pin(R1))].PCNTR3.POSR =
      (value & 0x01) ? (1 << bit_from_pin(arduino_pin_to_avr_pin(R1))) : 0;
  ((PORTS *)IO_PORT_START)->port[port_from_pin(arduino_pin_to_avr_pin(R1))].PCNTR3.PORR =
      (value & 0x01) ? 0 : (1 << bit_from_pin(arduino_pin_to_avr_pin(R1)));

  ((PORTS *)IO_PORT_START)->port[port_from_pin(arduino_pin_to_avr_pin(G1))].PCNTR3.POSR =
      (value & 0x02) ? (1 << bit_from_pin(arduino_pin_to_avr_pin(G1))) : 0;
  ((PORTS *)IO_PORT_START)->port[port_from_pin(arduino_pin_to_avr_pin(G1))].PCNTR3.PORR =
      (value & 0x02) ? 0 : (1 << bit_from_pin(arduino_pin_to_avr_pin(G1)));

  ((PORTS *)IO_PORT_START)->port[port_from_pin(arduino_pin_to_avr_pin(B1))].PCNTR3.POSR =
      (value & 0x04) ? (1 << bit_from_pin(arduino_pin_to_avr_pin(B1))) : 0;
  ((PORTS *)IO_PORT_START)->port[port_from_pin(arduino_pin_to_avr_pin(B1))].PCNTR3.PORR =
      (value & 0x04) ? 0 : (1 << bit_from_pin(arduino_pin_to_avr_pin(B1)));

  // Bottom half:
  ((PORTS *)IO_PORT_START)->port[port_from_pin(arduino_pin_to_avr_pin(R2))].PCNTR3.POSR =
      (value & 0x08) ? (1 << bit_from_pin(arduino_pin_to_avr_pin(R2))) : 0;
  ((PORTS *)IO_PORT_START)->port[port_from_pin(arduino_pin_to_avr_pin(R2))].PCNTR3.PORR =
      (value & 0x08) ? 0 : (1 << bit_from_pin(arduino_pin_to_avr_pin(R2)));

  ((PORTS *)IO_PORT_START)->port[port_from_pin(arduino_pin_to_avr_pin(G2))].PCNTR3.POSR =
      (value & 0x10) ? (1 << bit_from_pin(arduino_pin_to_avr_pin(G2))) : 0;
  ((PORTS *)IO_PORT_START)->port[port_from_pin(arduino_pin_to_avr_pin(G2))].PCNTR3.PORR =
      (value & 0x10) ? 0 : (1 << bit_from_pin(arduino_pin_to_avr_pin(G2)));

  ((PORTS *)IO_PORT_START)->port[port_from_pin(arduino_pin_to_avr_pin(B2))].PCNTR3.POSR =
      (value & 0x20) ? (1 << bit_from_pin(arduino_pin_to_avr_pin(B2))) : 0;
  ((PORTS *)IO_PORT_START)->port[port_from_pin(arduino_pin_to_avr_pin(B2))].PCNTR3.PORR =
      (value & 0x20) ? 0 : (1 << bit_from_pin(arduino_pin_to_avr_pin(B2)));
}

// ------------------------------------------------------------------------
// _stepRow
// Updates the row address lines (A, B, C, D) based on a global row variable.
// For a 4‑bit address (0–15), simply write each bit to the corresponding row pin.
// ------------------------------------------------------------------------
#ifndef PANEL_ROW_VAR
uint8_t _row = 0;
#define PANEL_ROW_VAR _row
#endif

#ifdef PANEL_MAX_SPEED
__attribute__((always_inline))
#endif
inline void _stepRow() {
  // For clarity, we use digitalWrite() here.
  // For maximum performance, replace these with direct port writes.
  digitalWrite(A, (PANEL_ROW_VAR & 0x01) ? HIGH : LOW);
  digitalWrite(B, (PANEL_ROW_VAR & 0x02) ? HIGH : LOW);
  digitalWrite(C, (PANEL_ROW_VAR & 0x04) ? HIGH : LOW);
  digitalWrite(D, (PANEL_ROW_VAR & 0x08) ? HIGH : LOW);
}

#endif // HUB75NANO_UNO_R4_METHODS_H
