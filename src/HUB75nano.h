/*
  Panel.h can be used to display stuff on a HUB75 panel with row shift registers with 64x32px in 8, 16 or 4096 colors
  designed for ARDUINO nano.
  (c) Leonhard Seidel, itsaps, jandlseb, 2023
  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 3 of the License, or (at your option) any later version.
  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.
  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/
#include <Arduino.h>
#include <avr/pgmspace.h>
#ifndef PANEL_H
#define PANEL_H

/////////////////////
// #define PANEL_BIG // use 2 bit rgb image buffer
// #define PANEL_FLASH // 4 bit flash buffer
// #define PANEL_NO_BUFFER // no buffer, immediate mode only
// #define PANEL_NO_FONT // disables everything font related, saves some flash
// #define PANEL_MAX_SPEED // aggresively inlines the 4 draw assembly instructions, else its kept as a method to keep size down
// #define PANEL_FLIP_VERTICAL // flips the panel vertically
// #define PANEL_FLIP_HORIZONTAL // flips the panel horizontally
/////////////////////

#pragma region definitions
// check we are on uno or nano
#ifndef ARDUINO_AVR_UNO
#ifndef ARDUINO_AVR_NANO
#error "This library only supports the Arduino nano and Uno, so the atm368p with 2kb sram, 1kb eeprom and 32kb flash. For other chips/boards, please see the internet or try to adapt this library here, but no guarantees"
#endif
#endif

// standard sizes
#ifndef PANEL_X
#define PANEL_X 64
#endif
#ifndef PANEL_Y
#define PANEL_Y 32
#endif

// buffer toggle
#ifdef PANEL_NO_BUFFER
#undef PANEL_FLASH
#undef PANEL_BIG
#define PANEL_NO_FONT
#endif

// sleep for brightnesses
#ifndef MAX_BRIGHTNESS_SLEEP_MUSEC
#define MAX_BRIGHTNESS_SLEEP_MUSEC 100
#endif
#ifndef BRIGHTNESS_SLEEP_MUSEC
#define BRIGHTNESS_SLEEP_MUSEC 40
#endif

// flash toggle
#ifdef PANEL_FLASH
#undef PANEL_BIG
#ifdef PANEL_FLIP_VERTICAL
#error "vertical flip cannot be done on flash output, only horizontal"
#endif

// have it bigger a size as we have more available lol
#define PANEL_BUFFERSIZE (PANEL_X * PANEL_Y * 2) // 4 byte per led, we have 6 bit per 2 led per color depth -> about 4k
#endif
#ifndef PANEL_BUFFERSIZE
#define PANEL_BUFFERSIZE (PANEL_X * PANEL_Y / 8)
#endif

// color transformatuion values
#ifndef MAX_COLORDEPTH
#ifdef PANEL_BIG
#define MAX_COLORDEPTH 2
#else
#ifdef PANEL_FLASH
#define MAX_COLORDEPTH 4
#else
#ifdef PANEL_NO_BUFFER
#define MAX_COLORDEPTH 8
#else
#define MAX_COLORDEPTH 1
#endif
#endif
#endif
#endif

#define MAX_COLOR (MAX_COLORDEPTH * MAX_COLORDEPTH - 1)
#define COLOR_CLAMP (255.0 / (MAX_COLOR))

// actual pin numbers like in the arduino digitalwrite, can be changed here or in your project
#ifndef RA
#define RA 14 // row selector a
#endif
#ifndef RB
#define RB 15 // row selector b
#endif
#ifndef RC
#define RC 16 // row selector c
#endif
#ifndef RD
#define RD 17 // row selector d
#endif
// currently unused
#ifndef RE
#define RE 18 // row selector e
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
#define CLK 9 // clock signal
#endif
#ifndef LAT
#define LAT 10 // data latch
#endif
#ifndef OE
#define OE 11 // output enable
#endif

// ref https://roboticsbackend.com/arduino-fast-digitalwrite/#Using_direct_port_manipulation_instead_of_digitalWrite
// helper definitions for setting/clearing
#define high_pin(port, number) port |= 1 << number
#define clear_pin(port, number) port &= ~(1 << number)

// arduino pin number to port number and the respective pin number conversion
#pragma region pin_to_port_number_conversion
// todo make script to generate these macros/do two preprocessor passes with makefile
// first row pin
#if RA < 8
#define PORT_RA PORTD
#define PORT_PIN_RA RA
#else
#if RA < 14
#define PORT_RA PORTB
#define PORT_PIN_RA RA - 8
#else
#define PORT_RA PORTC
#define PORT_PIN_RA RA - 14
#endif
#endif

// second row pin
#if RB < 8
#define PORT_RB PORTD
#define PORT_PIN_RB RB
#else
#if RB < 14
#define PORT_RB PORTB
#define PORT_PIN_RB RB - 8
#else
#define PORT_RB PORTC
#define PORT_PIN_RB RB - 14
#endif
#endif

// third row pin
#if RC < 8
#define PORT_RC PORTD
#define PORT_PIN_RC RC
#else
#if RC < 14
#define PORT_RC PORTB
#define PORT_PIN_RC RC - 8
#else
#define PORT_RC PORTC
#define PORT_PIN_RC RC - 14
#endif
#endif

// fourth row pin
#if RD < 8
#define PORT_RD PORTD
#define PORT_PIN_RD RC
#else
#if RD < 14
#define PORT_RD PORTB
#define PORT_PIN_RD RD - 8
#else
#define PORT_RD PORTC
#define PORT_PIN_RD RD - 14
#endif
#endif

// fifth row pin
#if RE < 8
#define PORT_RE PORTD
#define PORT_PIN_RE RE
#else
#if RE < 14
#define PORT_RE PORTB
#define PORT_PIN_RE RE - 8
#else
#define PORT_RE PORTC
#define PORT_PIN_RE RE - 14
#endif
#endif

// upper red
#if RF < 8
#define PORT_RF PORTD
#define PORT_PIN_RF RF
#else
#if RF < 14
#define PORT_RF PORTB
#define PORT_PIN_RF RF - 8
#else
#define PORT_RF PORTC
#define PORT_PIN_RF RF - 14
#endif
#endif

// upper green
#if GF < 8
#define PORT_GF PORTD
#define PORT_PIN_GF GF
#else
#if GF < 14
#define PORT_GF PORTB
#define PORT_PIN_GF GF - 8
#else
#define PORT_GF PORTC
#define PORT_PIN_GF GF - 14
#endif
#endif

// upper blue
#if BF < 8
#define PORT_BF PORTD
#define PORT_PIN_BF BF
#else
#if BF < 14
#define PORT_BF PORTB
#define PORT_PIN_BF BF - 8
#else
#define PORT_BF PORTC
#define PORT_PIN_BF BF - 14
#endif
#endif

// lower red
#if RS < 8
#define PORT_RS PORTD
#define PORT_PIN_RS RS
#else
#if RS < 14
#define PORT_RS PORTB
#define PORT_PIN_RS RS - 8
#else
#define PORT_RS PORTC
#define PORT_PIN_RS RS - 14
#endif
#endif

// lower green
#if GS < 8
#define PORT_GS PORTD
#define PORT_PIN_GS GS
#else
#if GS < 14
#define PORT_GS PORTB
#define PORT_PIN_GS GS - 8
#else
#define PORT_GS PORTC
#define PORT_PIN_GS GS - 14
#endif
#endif

// lower blue
#if BS < 8
#define PORT_BS PORTD
#define PORT_PIN_BS BS
#else
#if BS < 14
#define PORT_BS PORTB
#define PORT_PIN_BS BS - 8
#else
#define PORT_BS PORTC
#define PORT_PIN_BS BS - 14
#endif
#endif

// clock
#if CLK < 8
#define PORT_CLK PORTD
#define PORT_PIN_CLK CLK
#else
#if CLK < 14
#define PORT_CLK PORTB
#define PORT_PIN_CLK CLK - 8
#else
#define PORT_CLK PORTC
#define PORT_PIN_CLK CLK - 14
#endif
#endif

// latch
#if LAT < 8
#define PORT_LAT PORTD
#define PORT_PIN_LAT LAT
#else
#if LAT < 14
#define PORT_LAT PORTB
#define PORT_PIN_LAT LAT - 8
#else
#define PORT_LAT PORTC
#define PORT_PIN_LAT LAT - 14
#endif
#endif

// output enable
#if OE < 8
#define PORT_OE PORTD
#define PORT_PIN_OE OE
#else
#if OE < 14
#define PORT_OE PORTB
#define PORT_PIN_OE OE - 8
#else
#define PORT_OE PORTC
#define PORT_PIN_OE OE - 14
#endif
#endif

#pragma endregion // pin_to_port_number_conversion

// pin access defines, rest
#define HIGH_CLK high_pin(PORT_CLK, PORT_PIN_CLK)
#define CLEAR_CLK clear_pin(PORT_CLK, PORT_PIN_CLK)
#define HIGH_LAT high_pin(PORT_LAT, PORT_PIN_LAT)
#define CLEAR_LAT clear_pin(PORT_LAT, PORT_PIN_LAT)
#define HIGH_OE high_pin(PORT_OE, PORT_PIN_OE)
#define CLEAR_OE clear_pin(PORT_OE, PORT_PIN_OE)
#define Clock \
    HIGH_CLK; \
    CLEAR_CLK
#define LATCH \
    HIGH_LAT; \
    CLEAR_LAT

#pragma endregion // definitions

#pragma region color_helpers

#pragma pack(push, 1)
typedef union Color
{
    struct
    {
        uint8_t red : 4;
        uint8_t green : 4;
        uint8_t blue : 4;
        uint8_t invalid_bits : 4;
    };
    uint16_t color_444;
} Color;
#pragma pack(pop)

constexpr Color COLOR_888_to_444(uint8_t r, uint8_t g, uint8_t b)
{
    return {(r & (uint8_t)15), (g & (uint8_t)15), (b & (uint8_t)15)};
}
constexpr Color COLOR_888_to_444_FULL(uint8_t r, uint8_t g, uint8_t b)
{
    return {(int)(((double)r / 8) - 0.5), (int)(((double)g / 4) - 0.5), (int)(((double)b / 8) - 0.5)};
}
constexpr Color COLOR_888_to_444_CLAMPED(uint8_t r, uint8_t g, uint8_t b)
{
    return {(int)(((double)r / COLOR_CLAMP) + 0.5), (int)(((double)g / COLOR_CLAMP) + 0.5), (int)(((double)b / COLOR_CLAMP) + 0.5)};
}
constexpr Color COLOR_888_to_444_COLORF(float r, float g, float b)
{
    return {((int)(r * MAX_COLOR + 0.5f)), (int)((g * MAX_COLOR + 0.5f)), (int)((b * MAX_COLOR) + 0.5f)};
}
#pragma endregion

#pragma region font
#ifndef PANEL_NO_FONT
// Copyright <2010> <Robey Pointer, https://robey.lag.net/> =========>
//
// Permission is hereby granted, free of charge, to any person obtaining a copy of this softwareand associated documentation files(the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and /or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions :
// The above copyright noticeand this permission notice shall be included in all copies or substantial portions of the Software.
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//  Font Definition
const uint8_t font4x6[96][2] PROGMEM = {
    {0x00, 0x00}, /*SPACE*/
    {0x49, 0x08}, /*'!'*/
    {0xb4, 0x00}, /*'"'*/
    {0xbe, 0xf6}, /*'#'*/
    {0x7b, 0x7a}, /*'$'*/
    {0xa5, 0x94}, /*'%'*/
    {0x55, 0xb8}, /*'&'*/
    {0x48, 0x00}, /*'''*/
    {0x29, 0x44}, /*'('*/
    {0x44, 0x2a}, /*')'*/
    {0x15, 0xa0}, /*'*'*/
    {0x0b, 0x42}, /*'+'*/
    {0x00, 0x50}, /*','*/
    {0x03, 0x02}, /*'-'*/
    {0x00, 0x08}, /*'.'*/
    {0x25, 0x90}, /*'/'*/
    {0x76, 0xba}, /*'0'*/
    {0x59, 0x5c}, /*'1'*/
    {0xc5, 0x9e}, /*'2'*/
    {0xc5, 0x38}, /*'3'*/
    {0x92, 0xe6}, /*'4'*/
    {0xf3, 0x3a}, /*'5'*/
    {0x73, 0xba}, /*'6'*/
    {0xe5, 0x90}, /*'7'*/
    {0x77, 0xba}, /*'8'*/
    {0x77, 0x3a}, /*'9'*/
    {0x08, 0x40}, /*':'*/
    {0x08, 0x50}, /*';'*/
    {0x2a, 0x44}, /*'<'*/
    {0x1c, 0xe0}, /*'='*/
    {0x88, 0x52}, /*'>'*/
    {0xe5, 0x08}, /*'?'*/
    {0x56, 0x8e}, /*'@'*/
    {0x77, 0xb6}, /*'A'*/
    {0x77, 0xb8}, /*'B'*/
    {0x72, 0x8c}, /*'C'*/
    {0xd6, 0xba}, /*'D'*/
    {0x73, 0x9e}, /*'E'*/
    {0x73, 0x92}, /*'F'*/
    {0x72, 0xae}, /*'G'*/
    {0xb7, 0xb6}, /*'H'*/
    {0xe9, 0x5c}, /*'I'*/
    {0x64, 0xaa}, /*'J'*/
    {0xb7, 0xb4}, /*'K'*/
    {0x92, 0x9c}, /*'L'*/
    {0xbe, 0xb6}, /*'M'*/
    {0xd6, 0xb6}, /*'N'*/
    {0x56, 0xaa}, /*'O'*/
    {0xd7, 0x92}, /*'P'*/
    {0x76, 0xee}, /*'Q'*/
    {0x77, 0xb4}, /*'R'*/
    {0x71, 0x38}, /*'S'*/
    {0xe9, 0x48}, /*'T'*/
    {0xb6, 0xae}, /*'U'*/
    {0xb6, 0xaa}, /*'V'*/
    {0xb6, 0xf6}, /*'W'*/
    {0xb5, 0xb4}, /*'X'*/
    {0xb5, 0x48}, /*'Y'*/
    {0xe5, 0x9c}, /*'Z'*/
    {0x69, 0x4c}, /*'['*/
    {0x91, 0x24}, /*'\'*/
    {0x64, 0x2e}, /*']'*/
    {0x54, 0x00}, /*'^'*/
    {0x00, 0x1c}, /*'_'*/
    {0x44, 0x00}, /*'`'*/
    {0x0e, 0xae}, /*'a'*/
    {0x9a, 0xba}, /*'b'*/
    {0x0e, 0x8c}, /*'c'*/
    {0x2e, 0xae}, /*'d'*/
    {0x0e, 0xce}, /*'e'*/
    {0x56, 0xd0}, /*'f'*/
    {0x55, 0x3B}, /*'g'*/
    {0x93, 0xb4}, /*'h'*/
    {0x41, 0x44}, /*'i'*/
    {0x41, 0x51}, /*'j'*/
    {0x97, 0xb4}, /*'k'*/
    {0x49, 0x44}, /*'l'*/
    {0x17, 0xb6}, /*'m'*/
    {0x1a, 0xb6}, /*'n'*/
    {0x0a, 0xaa}, /*'o'*/
    {0xd6, 0xd3}, /*'p'*/
    {0x76, 0x67}, /*'q'*/
    {0x17, 0x90}, /*'r'*/
    {0x0f, 0x38}, /*'s'*/
    {0x9a, 0x8c}, /*'t'*/
    {0x16, 0xae}, /*'u'*/
    {0x16, 0xba}, /*'v'*/
    {0x16, 0xf6}, /*'w'*/
    {0x15, 0xb4}, /*'x'*/
    {0xb5, 0x2b}, /*'y'*/
    {0x1c, 0x5e}, /*'z'*/
    {0x6b, 0x4c}, /*'{'*/
    {0x49, 0x48}, /*'|'*/
    {0xc9, 0x5a}, /*'}'*/
    {0x54, 0x00}, /*'~'*/
    {0x56, 0xe2}  /*''*/
};
//<=============================================================================
//
// Copyright <2015> <https://hackaday.io/PK.3>========================>
//
// Permission is hereby granted, free of charge, to any person obtaining a copy of this softwareand associated documentation files(the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and /or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions :
// The above copyright noticeand this permission notice shall be included in all copies or substantial portions of the Software.
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
// Font retreival function - ugly, but needed.
// kindly stolen from https://hackaday.io/project/6309-vga-graphics-over-spi-and-serial-vgatonic/log/20759-a-tiny-4x6-pixel-font-that-will-fit-on-almost-any-microcontroller-license-mit#header
inline unsigned char getFontLine(unsigned char data, int line_num)
{
    const uint8_t index = (data - 32);
    unsigned char pixel = 0;
    if (pgm_read_byte(&font4x6[index][1]) & (uint8_t)1 == 1)
        line_num -= 1;
    if (line_num == 0)
    {
        pixel = (pgm_read_byte(&font4x6[index][0])) >> (uint8_t)4;
    }
    else if (line_num == 1)
    {
        pixel = (pgm_read_byte(&font4x6[index][0])) >> (uint8_t)1;
    }
    else if (line_num == 2)
    {
        // Split over 2 bytes
        return (((pgm_read_byte(&font4x6[index][0])) & (uint8_t)0x03) << (uint8_t)2) | (((pgm_read_byte(&font4x6[index][1])) & (uint8_t)0x02));
    }
    else if (line_num == 3)
    {
        pixel = (pgm_read_byte(&font4x6[index][1])) >> (uint8_t)4;
    }
    else if (line_num == 4)
    {
        pixel = (pgm_read_byte(&font4x6[index][1])) >> (uint8_t)1;
    }
    return pixel & (uint8_t)0xE;
} //<=============================================================================
#endif
#pragma endregion // font

#pragma region color_enum_definition
typedef class Colors
{
public:
    inline static const Color RED = COLOR_888_to_444(3, 0, 0);
    inline static const Color GREEN = COLOR_888_to_444(0, 3, 0);
    inline static const Color BLUE = COLOR_888_to_444(0, 0, 3);
    inline static const Color WHITE = COLOR_888_to_444(3, 3, 3);
    inline static const Color BLACK = COLOR_888_to_444(0, 0, 0);
    inline static const Color PURPLE = COLOR_888_to_444(3, 0, 3);
    inline static const Color YELLOW = COLOR_888_to_444(3, 3, 0);
    inline static const Color CYAN = COLOR_888_to_444(0, 3, 3);
#ifdef PANEL_BIG
    inline static const Color DARKRED = COLOR_888_to_444(2, 0, 0);
    inline static const Color DARKGREEN = COLOR_888_to_444(0, 2, 0);
    inline static const Color DARKBLUE = COLOR_888_to_444(0, 0, 2);
    inline static const Color DARKWHITE = COLOR_888_to_444(2, 2, 2);
    inline static const Color DARKPURPLE = COLOR_888_to_444(2, 0, 2);
    inline static const Color DARKYELLOW = COLOR_888_to_444(2, 2, 0);
    inline static const Color DARKCYAN = COLOR_888_to_444(0, 2, 2);
    inline static const Color DARKERRED = COLOR_888_to_444(1, 0, 0);
    inline static const Color DARKERGREEN = COLOR_888_to_444(0, 1, 0);
    inline static const Color DARKERBLUE = COLOR_888_to_444(0, 0, 1);
    inline static const Color DARKERWHITE = COLOR_888_to_444(1, 1, 1);
    inline static const Color DARKERPURPLE = COLOR_888_to_444(1, 0, 1);
    inline static const Color DARKERYELLOW = COLOR_888_to_444(1, 1, 0);
    inline static const Color DARKERCYAN = COLOR_888_to_444(0, 1, 1);
    inline static const Color ORANGE = COLOR_888_to_444(3, 1, 0);
#endif
    inline static const Color NO_COLOR = {0, 0, 0, 15};

} Colors;
#pragma endregion // color_enum_definition

class Panel
{
public:
#pragma region constructors
#ifdef PANEL_FLASH
    Panel(PGM_VOID_P buffer_in)
    {
        buffer = buffer_in;
        pinMode(RA, OUTPUT);
        pinMode(RB, OUTPUT);
        pinMode(RC, OUTPUT);
        pinMode(RD, OUTPUT);
        pinMode(RE, OUTPUT);
        pinMode(RF, OUTPUT);
        pinMode(RS, OUTPUT);
        pinMode(GF, OUTPUT);
        pinMode(GS, OUTPUT);
        pinMode(BF, OUTPUT);
        pinMode(BS, OUTPUT);
        pinMode(CLK, OUTPUT);
        pinMode(LAT, OUTPUT);
        pinMode(OE, OUTPUT);
    }
#else
    Panel()
    {
        pinMode(RA, OUTPUT);
        pinMode(RB, OUTPUT);
        pinMode(RC, OUTPUT);
        pinMode(RD, OUTPUT);
        pinMode(RE, OUTPUT);
        pinMode(RF, OUTPUT);
        pinMode(RS, OUTPUT);
        pinMode(GF, OUTPUT);
        pinMode(GS, OUTPUT);
        pinMode(BF, OUTPUT);
        pinMode(BS, OUTPUT);
        pinMode(CLK, OUTPUT);
        pinMode(LAT, OUTPUT);
        pinMode(OE, OUTPUT);
    }
#endif
#pragma endregion // constructors

#pragma region immediates
    void fillScreenColor(Color color)
    {
        for (uint8_t bitness = MAX_COLORDEPTH - 1; bitness < MAX_COLORDEPTH; bitness--)
        {
            set_color((((color.red >> (uint8_t)bitness) & (uint8_t)1) << (uint8_t)7) | (((color.green >> (uint8_t)bitness) & (uint8_t)1) << (uint8_t)6) | (((color.blue >> (uint8_t)bitness) & (uint8_t)1) << (uint8_t)5) | (((color.red >> (uint8_t)bitness) & (uint8_t)1) << (uint8_t)4) | (((color.green >> (uint8_t)bitness) & (uint8_t)1) << (uint8_t)3) | (((color.blue >> (uint8_t)bitness) & (uint8_t)1) << (uint8_t)2));

            // todo continue work here building it up, but not top priority
            for (uint8_t y = 0; y < PANEL_Y / 2; y++) // 32 rows
            {
                SendRow();
                LATCH;
                stepRow();
                CLEAR_OE;

#if MAX_BRIGHTNESS_SLEEP_MUSEC > 0
                delayMicroseconds(BRIGHTNESS_SLEEP_MUSEC >> bitness);
                if (BRIGHTNESS_SLEEP_MUSEC < MAX_BRIGHTNESS_SLEEP_MUSEC)
                {
                    HIGH_OE;
                    delayMicroseconds((MAX_BRIGHTNESS_SLEEP_MUSEC - BRIGHTNESS_SLEEP_MUSEC) >> bitness);
                }
#endif
            }
        }
    }
#pragma endregion // immediates

#pragma region led_struct_definition
#ifdef PANEL_BIG
#define LED LED_long
#else
#define LED LED_short
#endif
#pragma pack(1)
    typedef struct LED_short
    { // 3 bytes long, contains 8 leds at 1 bit color depth
        uint8_t redUpperBit1Led1 : 1;
        uint8_t greenUpperBit1Led1 : 1;
        uint8_t blueUpperBit1Led1 : 1;
        uint8_t redLowerBit1Led1 : 1;
        uint8_t greenLowerBit1Led1 : 1;
        uint8_t blueLowerBit1Led1 : 1;
        uint8_t redUpperBit1Led2 : 1;
        uint8_t greenUpperBit1Led2 : 1;
        uint8_t : 0;
        uint8_t blueUpperBit1Led2 : 1;
        uint8_t redLowerBit1Led2 : 1;
        uint8_t greenLowerBit1Led2 : 1;
        uint8_t blueLowerBit1Led2 : 1;
        uint8_t redUpperBit1Led3 : 1;
        uint8_t greenUpperBit1Led3 : 1;
        uint8_t blueUpperBit1Led3 : 1;
        uint8_t redLowerBit1Led3 : 1;
        uint8_t : 0;
        uint8_t greenLowerBit1Led3 : 1;
        uint8_t blueLowerBit1Led3 : 1;
        uint8_t redUpperBit1Led4 : 1;
        uint8_t greenUpperBit1Led4 : 1;
        uint8_t blueUpperBit1Led4 : 1;
        uint8_t redLowerBit1Led4 : 1;
        uint8_t greenLowerBit1Led4 : 1;
        uint8_t blueLowerBit1Led4 : 1;
        uint8_t : 0;
    } LED_short;
#pragma pack(1)
    typedef struct LED_long
    { // 6 bytes long, contains 8 leds at 2 bit color depth
        uint8_t redUpperBit1Led1 : 1;
        uint8_t greenUpperBit1Led1 : 1;
        uint8_t blueUpperBit1Led1 : 1;
        uint8_t redLowerBit1Led1 : 1;
        uint8_t greenLowerBit1Led1 : 1;
        uint8_t blueLowerBit1Led1 : 1;
        uint8_t redUpperBit2Led1 : 1;
        uint8_t greenUpperBit2Led1 : 1;
        uint8_t : 0;
        uint8_t blueUpperBit2Led1 : 1;
        uint8_t redLowerBit2Led1 : 1;
        uint8_t greenLowerBit2Led1 : 1;
        uint8_t blueLowerBit2Led1 : 1;
        uint8_t redUpperBit1Led2 : 1;
        uint8_t greenUpperBit1Led2 : 1;
        uint8_t blueUpperBit1Led2 : 1;
        uint8_t redLowerBit1Led2 : 1;
        uint8_t : 0;
        uint8_t greenLowerBit1Led2 : 1;
        uint8_t blueLowerBit1Led2 : 1;
        uint8_t redUpperBit2Led2 : 1;
        uint8_t greenUpperBit2Led2 : 1;
        uint8_t blueUpperBit2Led2 : 1;
        uint8_t redLowerBit2Led2 : 1;
        uint8_t greenLowerBit2Led2 : 1;
        uint8_t blueLowerBit2Led2 : 1;
        uint8_t : 0;
        uint8_t redUpperBit1Led3 : 1;
        uint8_t greenUpperBit1Led3 : 1;
        uint8_t blueUpperBit1Led3 : 1;
        uint8_t redLowerBit1Led3 : 1;
        uint8_t greenLowerBit1Led3 : 1;
        uint8_t blueLowerBit1Led3 : 1;
        uint8_t redUpperBit2Led3 : 1;
        uint8_t greenUpperBit2Led3 : 1;
        uint8_t : 0;
        uint8_t blueUpperBit2Led3 : 1;
        uint8_t redLowerBit2Led3 : 1;
        uint8_t greenLowerBit2Led3 : 1;
        uint8_t blueLowerBit2Led3 : 1;
        uint8_t redUpperBit1Led4 : 1;
        uint8_t greenUpperBit1Led4 : 1;
        uint8_t blueUpperBit1Led4 : 1;
        uint8_t redLowerBit1Led4 : 1;
        uint8_t : 0;
        uint8_t greenLowerBit1Led4 : 1;
        uint8_t blueLowerBit1Led4 : 1;
        uint8_t redUpperBit2Led4 : 1;
        uint8_t greenUpperBit2Led4 : 1;
        uint8_t blueUpperBit2Led4 : 1;
        uint8_t redLowerBit2Led4 : 1;
        uint8_t greenLowerBit2Led4 : 1;
        uint8_t blueLowerBit2Led4 : 1;
        uint8_t : 0;
    } LED_long;

#define LED_SHORT_MASK_UPPER_1 0b11000111
#define LED_SHORT_MASK_UPPER_2 0b01110001
#define LED_SHORT_MASK_UPPER_3 0b00011100
#define LED_SHORT_MASK_LOWER_1 ~LED_SHORT_MASK_UPPER_1
#define LED_SHORT_MASK_LOWER_2 ~LED_SHORT_MASK_UPPER_2
#define LED_SHORT_MASK_LOWER_3 ~LED_SHORT_MASK_UPPER_3
#define LED_LONG_MASK_UPPER_1 0b11000111
#define LED_LONG_MASK_UPPER_2 0b01110001
#define LED_LONG_MASK_UPPER_3 0b00011100
#define LED_LONG_MASK_UPPER_4 0b11000111
#define LED_LONG_MASK_UPPER_5 0b01110001
#define LED_LONG_MASK_UPPER_6 0b00011100
#define LED_LONG_MASK_LOWER_1 ~LED_LONG_MASK_UPPER_1
#define LED_LONG_MASK_LOWER_2 ~LED_LONG_MASK_UPPER_2
#define LED_LONG_MASK_LOWER_3 ~LED_LONG_MASK_UPPER_3
#define LED_LONG_MASK_LOWER_4 ~LED_LONG_MASK_UPPER_4
#define LED_LONG_MASK_LOWER_5 ~LED_LONG_MASK_UPPER_5
#define LED_LONG_MASK_LOWER_6 ~LED_LONG_MASK_UPPER_6
#pragma endregion // led_struct_definition

#pragma region buffer_copying
#ifdef PANEL_FLASH
    void swapBuffer(PGM_VOID_P newBuffer)
    {
        buffer = newBuffer;
    }
#else
    void swapBuffer(const LED *newBuffer, uint8_t bufferLength)
    {
        memcpy(buffer, newBuffer, bufferLength);
    }
#endif

    void fillBuffer(Color color)
    {
        // fills the buffer
        for (uint8_t x = 0; x < PANEL_X; x++)
        {
            for (uint8_t y = 0; y < PANEL_Y; y++)
            {
                setBuffer(x, y, color);
            }
        }
    }
#pragma endregion // buffer_copying

#pragma region drawing
    void drawLine(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, Color color)
    { // draws a line with color between the coords given
        // calculate both gradients
        int8_t dx = abs(x2 - x1);
        int8_t dy = -abs(y2 - y1);
        // create de-/increment values for walking the line
        int8_t sx = x1 < x2 ? 1 : -1;
        int8_t sy = y1 < y2 ? 1 : -1;
        // error corerction
        float err = dx + dy, e2;
        while (1)
        {
            setBuffer(x1, y1, color);
            e2 = 2 * err;
            if (e2 >= dy)
            {
                if (x1 == x2)
                    break;
                err += dy;
                x1 += sx;
            }
            if (e2 <= dx)
            {
                if (y1 == y2)
                    break;
                err += dx;
                y1 += sy;
            }
        }
    }

    void drawEllipse(uint8_t xm, uint8_t ym, uint8_t a, uint8_t b, Color color, bool fill)
    {
        int8_t x = -a;
        int8_t y = 0; /* II. quadrant from bottom left to top right */
        int16_t a2 = a * a;
        int16_t b2 = b * b;
        int16_t e2 = b * b;
        int16_t err = x * (2 * e2 + x) + e2; /* error of 1.step */
        do
        {
            setBuffer(xm - x, ym + y, color); /*   I. Quadrant */
            setBuffer(xm + x, ym + y, color); /*  II. Quadrant */
            setBuffer(xm + x, ym - y, color); /* III. Quadrant */
            setBuffer(xm - x, ym - y, color); /*  IV. Quadrant */
            e2 = 2 * err;
            if (e2 >= (x * 2 + 1) * b2) /* e_xy+e_x > 0 */
                err += (++x * 2 + 1) * b2;
            if (e2 <= (y * 2 + 1) * a2) /* e_xy+e_y < 0 */
                err += (++y * 2 + 1) * a2;
        } while (x <= 0);
        while (y++ < b)
        {                                 /* to early stop of flat ellipses a=1, */
            setBuffer(xm, ym + y, color); /* -> finish tip of ellipse */
            setBuffer(xm, ym - y, color);
        }
        if (fill)
        {
            while (a > 0)
            {
                drawEllipse(xm, ym, --a, b, color, true);
            }
        }
    }

    void drawCircle(uint8_t xm, uint8_t ym, uint8_t radius, Color color, bool fill)
    {
        // draws a circle at the coords with radius and color
        int8_t x = -radius;
        int8_t y = 0;
        int8_t new_radius = radius;
        int16_t err = 2 - 2 * radius; // bottom left to top right
        do
        {
            setBuffer(xm - x, ym + y, color); //   I. Quadrant +x +y
            setBuffer(xm - y, ym - x, color); //  II. Quadrant -x +y
            setBuffer(xm + x, ym - y, color); // III. Quadrant -x -y
            setBuffer(xm + y, ym + x, color); //  IV. Quadrant +x -y
            new_radius = err;
            if (new_radius <= y)
                err += ++y * 2 + 1;        // e_xy+e_y < 0
            if (new_radius > x || err > y) // e_xy+e_x > 0 or no 2nd y-step
                err += ++x * 2 + 1;        // -> x-step now
        } while (x < 0);

        if (fill) // fill works
        {
            // check if point in circle, then fill
            for (int8_t i = -radius; i < radius; i++)
            {
                for (int8_t j = -radius; j < radius; j++)
                {
                    if (((int16_t)i * i + (int16_t)j * j) < ((radius - 0.5) * (radius - 0.5)))
                    {
                        setBuffer(xm + i, ym + j, color);
                    }
                }
            }
        }
    }

    void drawRect(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, Color color, bool fill)
    {
        // draws a rect filled ro not filled with the given color at coords (landscape, origin in upper left corner)
        // get colors
        if (fill)
        { // vertical lines

            if (x1 == x2) // 1 wide ones
            {
                for (uint8_t j = y1; j <= y2; j++)
                {
                    setBuffer(x1, j, color);
                }
                return;
            }
            else if (x2 - x1 == 1) // 2 wide
            {
                for (uint8_t j = y1; j <= y2; j++)
                {
                    setBuffer(x1, j, color);
                    setBuffer(x1 + 1, j, color);
                }
                return;
            }
            else if (x2 - x1 == 2) // 3 wide
            {
                for (uint8_t j = y1; j <= y2; j++)
                {
                    setBuffer(x1, j, color);
                    setBuffer(x1 + 1, j, color);
                    setBuffer(x1 + 2, j, color);
                }
                return;
            }

            // go through each horizontal line and fill (try for blocks of 4 led)
            uint8_t original_x1 = x1, original_x2 = x2 + 1;

            for (uint8_t j = y1; j <= y2; j++)
            { // horizontal lines
                x1 = original_x1;
                x2 = original_x2;
                while ((x1 & 3) != 0 && x1 < x2)
                {
                    setBuffer(x1, j, color);
                    x1++;
                }

                while ((x2 & 3) != 0 && x2 > x1)
                {
                    x2--;
                    setBuffer(x2, j, color);
                }

                // check against overflow
                if (x2 - x1 >= 4 && !(SREG & _BV(SREG_C)))
                {
                    uint8_t block_count = ((x2 - x1) / 4);

                    setBuffer4xBlockWise(x1, j, block_count, color);
                }
            }
        }
        else
        {
            // vertical lines
            if (x1 == x2) // 1 wide ones
            {
                for (uint8_t j = y1; j <= y2; j++)
                {
                    setBuffer(x1, j, color);
                }
                return;
            }

            for (uint8_t j = y1; j <= y2; j++)
            {
                setBuffer(x1, j, color);
                setBuffer(x2, j, color);
            }

            x1++;
            //  guard against rects beign 1 high
            if (y1 == y2)
            {
                // short horizontal lines
                while ((x1 & 3) != 0 && x1 < x2)
                {
                    setBuffer(x1, y1, color);
                    x1++;
                }

                while ((x2 & 3) != 0 && x2 > x1)
                {
                    x2--;
                    setBuffer(x2, y1, color);
                }

                // blocks of horizontal lines
                if (x2 - x1 >= 4 && !(SREG & _BV(SREG_C)))
                {
                    uint8_t block_count = ((x2 - x1) / 4);
                    setBuffer4xBlockWise(x1, y1, block_count, color);
                }
            }
            else
            {
                // short horizontal lines
                while ((x1 & 3) != 0 && x1 < x2)
                {
                    setBuffer(x1, y1, color);
                    setBuffer(x1, y2, color);
                    x1++;
                }

                while ((x2 & 3) != 0 && x2 > x1)
                {
                    x2--;
                    setBuffer(x2, y1, color);
                    setBuffer(x2, y2, color);
                }

                // blocks of horizontal lines
                if (x2 - x1 >= 4 && !(SREG & _BV(SREG_C)))
                {
                    uint8_t block_count = ((x2 - x1) / 4);

                    setBuffer4xBlockWise(x1, y1, block_count, color);
                    setBuffer4xBlockWise(x1, y2, block_count, color);
                }
            }
        }
    }

#ifdef PANEL_MAX_SPEED
    __attribute__((always_inline)) inline void drawSquare(uint8_t x, uint8_t y, uint8_t size, Color color, bool fill)
#else
    inline void drawSquare(uint8_t x, uint8_t y, uint8_t size, Color color, bool fill)
#endif
    {
        size--;
        drawRect(x, y, x + size, y + size, color, fill);
    }

#ifndef PANEL_NO_FONT

#ifdef PANEL_MAX_SPEED
    __attribute__((always_inline)) inline void drawChar(uint8_t x, uint8_t y, char letter, Color color)
#else
    inline void drawChar(uint8_t x, uint8_t y, char letter, Color color)
#endif
    {
        drawChar(x, y, letter, color, Colors::NO_COLOR);
    }

    void drawChar(uint8_t x, uint8_t y, char letter, Color color, const Color bg_color)
    {
        // iterate through the character line by line
        for (uint8_t i = 0; i < 5; i++)
        {
            char current_line = getFontLine(letter, i);
            // iterate through the character bit by bit
            for (uint8_t j = 0; j < 4; j++)
            {
                // shift by j and check for bit set
                if (current_line & (uint8_t)(1 << (uint8_t)(4 - j)))
                {
                    // set pixel at i and j
                    setBuffer(x + j, y + i, color);
                }
                else if (bg_color.invalid_bits == 0)
                {
                    setBuffer(x + j, y + i, bg_color);
                }
            }
        }
    }

#ifdef PANEL_MAX_SPEED
    __attribute__((always_inline)) inline void drawBigChar(uint8_t x, uint8_t y, char letter, Color color, uint8_t size_modifier)
#else
    inline void drawBigChar(uint8_t x, uint8_t y, char letter, Color color, uint8_t size_modifier)
#endif
    {
        if (size_modifier == 1)
        {
            drawChar(x, y, letter, color, Colors::NO_COLOR);
            return;
        }
        drawBigChar(x, y, letter, color, Colors::NO_COLOR, size_modifier);
    }

    void drawBigChar(uint8_t x, uint8_t y, char letter, Color color, Color bg_color, uint8_t size_modifier)
    { // new with scaling, but may be slower
        if (size_modifier == 1)
        {
            drawChar(x, y, letter, color, bg_color);
            return;
        }

        // iterate through the character line by line
        for (uint8_t i = 0; i < 5; i++)
        {
            char current_line = getFontLine(letter, i);
            // iterate through the character bit by bit, so x direction
            for (uint8_t j = 0; j < 4; j++)
            {
                // shift by j and check for bit set
                if (current_line & (uint8_t)(1 << (uint8_t)(4 - j)))
                {
                    // set pixel at i and j
                    drawSquare(x + j * size_modifier, y + i * size_modifier, size_modifier, color, true);
                }
                else if (bg_color.invalid_bits == 0)
                {
                    drawSquare(x + j * size_modifier, y + i * size_modifier, size_modifier, bg_color, true);
                }
            }
        }
    }

#ifdef PANEL_MAX_SPEED
    __attribute__((always_inline)) inline void drawString(uint8_t x, uint8_t y, char *str, Color color, Color bg_color)
#else
    inline void drawString(uint8_t x, uint8_t y, char *str, Color color, Color bg_color)
#endif
    {
        drawString(x, y, str, color, bg_color, 1);
    }

#ifdef PANEL_MAX_SPEED
    __attribute__((always_inline)) inline void drawString(uint8_t x, uint8_t y, char *str, Color color)
#else
    inline void drawString(uint8_t x, uint8_t y, char *str, Color color)
#endif
    {
        drawString(x, y, str, color, Colors::NO_COLOR, 1);
    }

#ifdef PANEL_MAX_SPEED
    __attribute__((always_inline)) inline void drawString(uint8_t x, uint8_t y, char *str, Color color, uint8_t size_modifier)
#else
    inline void drawString(uint8_t x, uint8_t y, char *str, Color color, uint8_t size_modifier)
#endif
    {
        drawString(x, y, str, color, Colors::NO_COLOR, size_modifier);
    }

    void drawString(uint8_t x, uint8_t y, char *str, Color color, Color bg_color, uint8_t size_modifier)
    {
        for (uint8_t i = 0; i < strlen(str); i++)
        {
            uint8_t xoffset = x + (3 * size_modifier + 1) * i;
            if (xoffset > PANEL_X)
                return;

            drawBigChar(xoffset, y, str[i], color, bg_color, size_modifier); // seperate by 1
            // drawBigChar(x + 4 * size_modifier * i, y, str[i], color, size_modifier); //seperate by size
        }
    }
#endif

// todo add drawing methods for
// - triangle with arbitrary points
// - triangle with right angle, rotation and side lengths
// - line with given width
#pragma endregion // drawing

#pragma region buffer_output_definitions:

#ifdef PANEL_MAX_SPEED
    __attribute__((always_inline)) inline void displayBuffer()
#else
    inline void displayBuffer()
#endif
    {
        // puts the  buffer contents onto the panel
#ifdef PANEL_BIG
        displayBigBuffer(); // 1 bit buffer in ram
#else
#ifdef PANEL_FLASH
        displayFlashBuffer(); // 4 bit buffer in flash
#else
        displaySmallBuffer(); // 2 bit buffer in ram
#endif
#endif
    }
#pragma endregion // buffer_output_definitions

#ifdef PANEL_MAX_SPEED
    __attribute__((always_inline)) inline void setBuffer(uint8_t x, uint8_t y, Color color)
#else
    inline void setBuffer(uint8_t x, uint8_t y, Color color)
#endif
    {
#ifdef PANEL_BIG
        setBigBuffer(x, y, color); // 1 bit buffer in ram
#else
#ifndef PANEL_FLASH
        setSmallBuffer(x, y, color); // 2 bit buffer in ram
#else
#endif
#endif
    }

private:
    uint8_t row = 0;

#pragma region buffer_definition
#ifndef PANEL_NO_BUFFER
#ifdef PANEL_BIG
    LED buffer[PANEL_BUFFERSIZE]; // uses 768 bytes on max size display with 1 bit, 1536 bytes with 2 bits of depth
#else
#ifdef PANEL_FLASH
    PGM_VOID_P buffer = 0;
#else
    LED buffer[PANEL_BUFFERSIZE];
#endif
#endif
#else
    LED buffer[0];
#endif
#pragma endregion // buffer_definition

#pragma region buffer_setting_definitions:

#ifdef PANEL_MAX_SPEED
    __attribute__((always_inline)) inline void setBuffer4xBlockWise(uint8_t x, uint8_t y, uint8_t block_count, Color color)
#else
    inline void setBuffer4xBlockWise(uint8_t x, uint8_t y, uint8_t block_count, Color color)
#endif
    {
#ifdef PANEL_BIG
        setBigBuffer4x(x, y, block_count, color); // 1 bit buffer in ram
#else
#ifndef PANEL_FLASH
        setSmallBuffer4x(x, y, block_count, color); // 2 bit buffer in ram
#else
#endif
#endif
    }
#pragma endregion // buffer_setting_definitions

#pragma GCC push_options
#pragma GCC optimize("unroll-loops")
#ifdef PANEL_MAX_SPEED
    __attribute__((always_inline)) inline void SendRow()
#else
    inline void SendRow()
#endif
    {
        for (uint8_t i = 0; i < PANEL_X; i++)
        {
            Clock;
        }
    }
#pragma GCC pop_options

// we can only set the row fast when the pins are in order
#ifdef PANEL_MAX_SPEED
    __attribute__((always_inline)) inline void stepRow()
    {
#else
    inline void stepRow()
    {
#endif

#if RA == 14 and RB == 15 and RC == 16 and RD == 17
        // set the 4 row pins at once
        PORTC = row & (uint8_t)31 | PORTC & (uint8_t)224;
#else
        __asm__ __volatile__("sbrc	%0, 0" ::"r"(row));
        high_pin(PORT_RA, PORT_PIN_RA);
        __asm__ __volatile__("sbrs	%0, 0" ::"r"(row));
        clear_pin(PORT_RA, PORT_PIN_RA);
        __asm__ __volatile__("sbrc	%0, 1" ::"r"(row));
        high_pin(PORT_RB, PORT_PIN_RB);
        __asm__ __volatile__("sbrs	%0, 1" ::"r"(row));
        clear_pin(PORT_RB, PORT_PIN_RB);
        __asm__ __volatile__("sbrc	%0, 2" ::"r"(row));
        high_pin(PORT_RC, PORT_PIN_RC);
        __asm__ __volatile__("sbrs	%0, 2" ::"r"(row));
        clear_pin(PORT_RC, PORT_PIN_RC);
        __asm__ __volatile__("sbrc	%0, 3" ::"r"(row));
        high_pin(PORT_RD, PORT_PIN_RD);
        __asm__ __volatile__("sbrs	%0, 3" ::"r"(row));
        clear_pin(PORT_RD, PORT_PIN_RD);
#endif
        row = (row + 1) & (uint8_t)31;
    }

// bulk pin access color, only good if pins are in right order
#ifdef PANEL_MAX_SPEED
    __attribute__((always_inline)) inline void set_color(uint8_t value){
#else
    inline void set_color(uint8_t value)
    {
#endif
#if RF == 2 and GF == 3 and BF == 4 and RS == 5 and GS == 6 and BS == 7
        // set 6 color pins and keep the rx tx pins as are
        PORTD = value | (PORTD & (uint8_t)3);
#else
        __asm__ __volatile__("sbrc	%0, 2" ::"r"(value));
        high_pin(PORT_RF, PORT_PIN_RF);
        __asm__ __volatile__("sbrs	%0, 2" ::"r"(value));
        clear_pin(PORT_RF, PORT_PIN_RF);
        __asm__ __volatile__("sbrc	%0, 3" ::"r"(value));
        high_pin(PORT_GF, PORT_PIN_GF);
        __asm__ __volatile__("sbrs	%0, 3" ::"r"(value));
        clear_pin(PORT_GF, PORT_PIN_GF);
        __asm__ __volatile__("sbrc	%0, 4" ::"r"(value));
        high_pin(PORT_BF, PORT_PIN_BF);
        __asm__ __volatile__("sbrs	%0, 4" ::"r"(value));
        clear_pin(PORT_BF, PORT_PIN_BF);
        __asm__ __volatile__("sbrc	%0, 5" ::"r"(value));
        high_pin(PORT_RS, PORT_PIN_RS);
        __asm__ __volatile__("sbrs	%0, 5" ::"r"(value));
        clear_pin(PORT_RS, PORT_PIN_RS);
        __asm__ __volatile__("sbrc	%0, 6" ::"r"(value));
        high_pin(PORT_GS, PORT_PIN_GS);
        __asm__ __volatile__("sbrs	%0, 6" ::"r"(value));
        clear_pin(PORT_GS, PORT_PIN_GS);
        __asm__ __volatile__("sbrc	%0, 7" ::"r"(value));
        high_pin(PORT_BS, PORT_PIN_BS);
        __asm__ __volatile__("sbrs	%0, 7" ::"r"(value));
        clear_pin(PORT_BS, PORT_PIN_BS);
#endif
}

#pragma region buffer_specifics
#ifndef PANEL_FLASH
#ifndef PANEL_BIG
void
displaySmallBuffer()
{
    LED *index;
    CLEAR_OE;
    for (uint8_t y = 0; y < PANEL_Y / 2; y++) // 16 rows
    {
        index = (LED *)(&buffer) + (y << (uint8_t)4);
        // we set each pixel after the other

        set_color(*(uint8_t *)(index) << (uint8_t)2);
        Clock;
        set_color((uint8_t)((*((uint16_t *)(index)) >> (uint8_t)4)));
        Clock;
        set_color((uint8_t)((*((uint16_t *)(((uint8_t *)(index) + sizeof(uint8_t))))) >> (uint8_t)2));
        Clock;
        set_color((*(((uint8_t *)(index)) + (sizeof(uint8_t) * 2))));
        Clock;
        ++index;
        set_color(*(uint8_t *)(index) << (uint8_t)2);
        Clock;
        set_color((uint8_t)((*((uint16_t *)(index)) >> (uint8_t)4)));
        Clock;
        set_color((uint8_t)((*((uint16_t *)(((uint8_t *)(index) + sizeof(uint8_t))))) >> (uint8_t)2));
        Clock;
        set_color((*(((uint8_t *)(index)) + (sizeof(uint8_t) * 2))));
        Clock;

        ++index;
        set_color(*(uint8_t *)(index) << (uint8_t)2);
        Clock;
        set_color((uint8_t)((*((uint16_t *)(index)) >> (uint8_t)4)));
        Clock;
        set_color((uint8_t)((*((uint16_t *)(((uint8_t *)(index) + sizeof(uint8_t))))) >> (uint8_t)2));
        Clock;
        set_color((*(((uint8_t *)(index)) + (sizeof(uint8_t) * 2))));
        Clock;
        ++index;
        set_color(*(uint8_t *)(index) << (uint8_t)2);
        Clock;
        set_color((uint8_t)((*((uint16_t *)(index)) >> (uint8_t)4)));
        Clock;
        set_color((uint8_t)((*((uint16_t *)(((uint8_t *)(index) + sizeof(uint8_t))))) >> (uint8_t)2));
        Clock;
        set_color((*(((uint8_t *)(index)) + (sizeof(uint8_t) * 2))));
        Clock;

        ++index;
        set_color(*(uint8_t *)(index) << (uint8_t)2);
        Clock;
        set_color((uint8_t)((*((uint16_t *)(index)) >> (uint8_t)4)));
        Clock;
        set_color((uint8_t)((*((uint16_t *)(((uint8_t *)(index) + sizeof(uint8_t))))) >> (uint8_t)2));
        Clock;
        set_color((*(((uint8_t *)(index)) + (sizeof(uint8_t) * 2))));
        Clock;
        ++index;
        set_color(*(uint8_t *)(index) << (uint8_t)2);
        Clock;
        set_color((uint8_t)((*((uint16_t *)(index)) >> (uint8_t)4)));
        Clock;
        set_color((uint8_t)((*((uint16_t *)(((uint8_t *)(index) + sizeof(uint8_t))))) >> (uint8_t)2));
        Clock;
        set_color((*(((uint8_t *)(index)) + (sizeof(uint8_t) * 2))));
        Clock;

        ++index;
        set_color(*(uint8_t *)(index) << (uint8_t)2);
        Clock;
        set_color((uint8_t)((*((uint16_t *)(index)) >> (uint8_t)4)));
        Clock;
        set_color((uint8_t)((*((uint16_t *)(((uint8_t *)(index) + sizeof(uint8_t))))) >> (uint8_t)2));
        Clock;
        set_color((*(((uint8_t *)(index)) + (sizeof(uint8_t) * 2))));
        Clock;
        ++index;
        set_color(*(uint8_t *)(index) << (uint8_t)2);
        Clock;
        set_color((uint8_t)((*((uint16_t *)(index)) >> (uint8_t)4)));
        Clock;
        set_color((uint8_t)((*((uint16_t *)(((uint8_t *)(index) + sizeof(uint8_t))))) >> (uint8_t)2));
        Clock;
        set_color((*(((uint8_t *)(index)) + (sizeof(uint8_t) * 2))));
        Clock;

        ++index;
        set_color(*(uint8_t *)(index) << (uint8_t)2);
        Clock;
        set_color((uint8_t)((*((uint16_t *)(index)) >> (uint8_t)4)));
        Clock;
        set_color((uint8_t)((*((uint16_t *)(((uint8_t *)(index) + sizeof(uint8_t))))) >> (uint8_t)2));
        Clock;
        set_color((*(((uint8_t *)(index)) + (sizeof(uint8_t) * 2))));
        Clock;
        ++index;
        set_color(*(uint8_t *)(index) << (uint8_t)2);
        Clock;
        set_color((uint8_t)((*((uint16_t *)(index)) >> (uint8_t)4)));
        Clock;
        set_color((uint8_t)((*((uint16_t *)(((uint8_t *)(index) + sizeof(uint8_t))))) >> (uint8_t)2));
        Clock;
        set_color((*(((uint8_t *)(index)) + (sizeof(uint8_t) * 2))));
        Clock;

        ++index;
        set_color(*(uint8_t *)(index) << (uint8_t)2);
        Clock;
        set_color((uint8_t)((*((uint16_t *)(index)) >> (uint8_t)4)));
        Clock;
        set_color((uint8_t)((*((uint16_t *)(((uint8_t *)(index) + sizeof(uint8_t))))) >> (uint8_t)2));
        Clock;
        set_color((*(((uint8_t *)(index)) + (sizeof(uint8_t) * 2))));
        Clock;
        ++index;
        set_color(*(uint8_t *)(index) << (uint8_t)2);
        Clock;
        set_color((uint8_t)((*((uint16_t *)(index)) >> (uint8_t)4)));
        Clock;
        set_color((uint8_t)((*((uint16_t *)(((uint8_t *)(index) + sizeof(uint8_t))))) >> (uint8_t)2));
        Clock;
        set_color((*(((uint8_t *)(index)) + (sizeof(uint8_t) * 2))));
        Clock;

        ++index;
        set_color(*(uint8_t *)(index) << (uint8_t)2);
        Clock;
        set_color((uint8_t)((*((uint16_t *)(index)) >> (uint8_t)4)));
        Clock;
        set_color((uint8_t)((*((uint16_t *)(((uint8_t *)(index) + sizeof(uint8_t))))) >> (uint8_t)2));
        Clock;
        set_color((*(((uint8_t *)(index)) + (sizeof(uint8_t) * 2))));
        Clock;
        ++index;
        set_color(*(uint8_t *)(index) << (uint8_t)2);
        Clock;
        set_color((uint8_t)((*((uint16_t *)(index)) >> (uint8_t)4)));
        Clock;
        set_color((uint8_t)((*((uint16_t *)(((uint8_t *)(index) + sizeof(uint8_t))))) >> (uint8_t)2));
        Clock;
        set_color((*(((uint8_t *)(index)) + (sizeof(uint8_t) * 2))));
        Clock;

        ++index;
        set_color(*(uint8_t *)(index) << (uint8_t)2);
        Clock;
        set_color((uint8_t)((*((uint16_t *)(index)) >> (uint8_t)4)));
        Clock;
        set_color((uint8_t)((*((uint16_t *)(((uint8_t *)(index) + sizeof(uint8_t))))) >> (uint8_t)2));
        Clock;
        set_color((*(((uint8_t *)(index)) + (sizeof(uint8_t) * 2))));
        Clock;
        ++index;
        set_color(*(uint8_t *)(index) << (uint8_t)2);
        Clock;
        set_color((uint8_t)((*((uint16_t *)(index)) >> (uint8_t)4)));
        Clock;
        set_color((uint8_t)((*((uint16_t *)(((uint8_t *)(index) + sizeof(uint8_t))))) >> (uint8_t)2));
        Clock;
        set_color((*(((uint8_t *)(index)) + (sizeof(uint8_t) * 2))));
        Clock;
        // set row
        HIGH_OE;
        LATCH;
        stepRow();
        CLEAR_OE;
    }
    HIGH_OE;
}

#ifndef PANEL_FLASH
void setSmallBuffer(uint8_t x, uint8_t y, Color color)
{
#ifdef PANEL_FLIP_VERTICAL
    y = PANEL_Y - y;
#endif
#ifdef PANEL_FLIP_HORIZONTAL
    x = PANEL_X - x;
#endif
    if (y < (PANEL_Y / 2))
    {
        // we are in upper half of pixels
        uint16_t index = ((y * PANEL_X) + x) / 4;
        switch (x & 3)
        {
        case 0: /*first pixel*/
            buffer[index].redUpperBit1Led1 = color.red;
            buffer[index].greenUpperBit1Led1 = color.green;
            buffer[index].blueUpperBit1Led1 = color.blue;
            break;
        case 1: /*second pixel*/
            buffer[index].redUpperBit1Led2 = color.red;
            buffer[index].greenUpperBit1Led2 = color.green;
            buffer[index].blueUpperBit1Led2 = color.blue;
            break;
        case 2: /*third pixel*/
            buffer[index].redUpperBit1Led3 = color.red;
            buffer[index].greenUpperBit1Led3 = color.green;
            buffer[index].blueUpperBit1Led3 = color.blue;
            break;
        case 3: /*fourth pixel*/
            buffer[index].redUpperBit1Led4 = color.red;
            buffer[index].greenUpperBit1Led4 = color.green;
            buffer[index].blueUpperBit1Led4 = color.blue;
            break;
        default:
            break;
        }
    }
    else
    {
        y -= (PANEL_Y / 2);
        // we are in lower half of pixels
        uint16_t index = (y * PANEL_X + x) / 4;
        switch (x & 3)
        {
        case 0: /*first pixel*/
            buffer[index].redLowerBit1Led1 = color.red;
            buffer[index].greenLowerBit1Led1 = color.green;
            buffer[index].blueLowerBit1Led1 = color.blue;
            break;
        case 1: /*second pixel*/
            buffer[index].redLowerBit1Led2 = color.red;
            buffer[index].greenLowerBit1Led2 = color.green;
            buffer[index].blueLowerBit1Led2 = color.blue;
            break;
        case 2: /*third pixel*/
            buffer[index].redLowerBit1Led3 = color.red;
            buffer[index].greenLowerBit1Led3 = color.green;
            buffer[index].blueLowerBit1Led3 = color.blue;
            break;
        case 3: /*fourth pixel*/
            buffer[index].redLowerBit1Led4 = color.red;
            buffer[index].greenLowerBit1Led4 = color.green;
            buffer[index].blueLowerBit1Led4 = color.blue;
            break;
        default:
            break;
        }
    }
}

void setSmallBuffer4x(uint8_t x, uint8_t y, uint8_t block_count, Color color)
{
#ifdef PANEL_FLIP_VERTICAL
    y = PANEL_Y - y;
#endif
#ifdef PANEL_FLIP_HORIZONTAL
    x = PANEL_X - x;
#endif
    if (y < (PANEL_Y / 2))
    {
        // we are in upper half of pixels
        uint16_t index = ((y * PANEL_X) + x) / 4;

        buffer[index].redUpperBit1Led1 = color.red;
        buffer[index].greenUpperBit1Led1 = color.green;
        buffer[index].blueUpperBit1Led1 = color.blue;
        buffer[index].redUpperBit1Led2 = color.red;
        buffer[index].greenUpperBit1Led2 = color.green;
        buffer[index].blueUpperBit1Led2 = color.blue;
        buffer[index].redUpperBit1Led3 = color.red;
        buffer[index].greenUpperBit1Led3 = color.green;
        buffer[index].blueUpperBit1Led3 = color.blue;
        buffer[index].redUpperBit1Led4 = color.red;
        buffer[index].greenUpperBit1Led4 = color.green;
        buffer[index].blueUpperBit1Led4 = color.blue;

        // temp buffers to store the cleaned values
        uint8_t lower, mid, higher;
        uint8_t *start = (uint8_t *)&buffer[index];

        // clean and cache the bits needed for thsi half
        lower = ((*start) & LED_SHORT_MASK_UPPER_1);
        start++;
        mid = ((*start) & LED_SHORT_MASK_UPPER_2);
        start++;
        higher = ((*start) & LED_SHORT_MASK_UPPER_3);
        start++;

        for (uint8_t i = 1; i < block_count; i++)
        {
            // apply the masks so we only copy the one half of the pixels we are "allowed" to
            *start &= LED_SHORT_MASK_LOWER_1;
            *start |= lower;
            start++;
            *start &= LED_SHORT_MASK_LOWER_2;
            *start |= mid;
            start++;
            *start &= LED_SHORT_MASK_LOWER_3;
            *start |= higher;
            start++;
        }
    }
    else
    {
        y -= (PANEL_Y / 2);
        // we are in lower half of pixels
        uint16_t index = ((y * PANEL_X) + x) / 4;

        buffer[index].redLowerBit1Led1 = color.red;
        buffer[index].greenLowerBit1Led1 = color.green;
        buffer[index].blueLowerBit1Led1 = color.blue;
        buffer[index].redLowerBit1Led2 = color.red;
        buffer[index].greenLowerBit1Led2 = color.green;
        buffer[index].blueLowerBit1Led2 = color.blue;
        buffer[index].redLowerBit1Led3 = color.red;
        buffer[index].greenLowerBit1Led3 = color.green;
        buffer[index].blueLowerBit1Led3 = color.blue;
        buffer[index].redLowerBit1Led4 = color.red;
        buffer[index].greenLowerBit1Led4 = color.green;
        buffer[index].blueLowerBit1Led4 = color.blue;

        // temp buffers to store the cleaned values
        uint8_t lower, mid, higher;
        uint8_t *start = (uint8_t *)&buffer[index];

        // clean and cache the bits needed for thsi half
        lower = ((*start) & LED_SHORT_MASK_LOWER_1);
        start++;
        mid = ((*start) & LED_SHORT_MASK_LOWER_2);
        start++;
        higher = ((*start) & LED_SHORT_MASK_LOWER_3);
        start++;

        for (uint8_t i = 1; i < block_count; i++)
        {
            // apply the masks so we only copy the one half of the pixels we are "allowed" to
            *start &= LED_SHORT_MASK_UPPER_1;
            *start |= lower;
            start++;
            *start &= LED_SHORT_MASK_UPPER_2;
            *start |= mid;
            start++;
            *start &= LED_SHORT_MASK_UPPER_3;
            *start |= higher;
            start++;
        }
    }
}
#endif

#else
void
displayBigBuffer()
{
    LED *index;

    // coding:
    // 00 -> off
    // 01 -> 33%
    // 10 -> 66%
    // 11 -> on
    // msb

    for (uint8_t y = 0; y < PANEL_Y / 2; y++) // 16 rows
    {
        index = (LED *)(&buffer) + (y << (uint8_t)4);

        set_color((uint8_t)((*((uint16_t *)(index)) >> (uint8_t)4)));
        Clock;
        set_color((*(((uint8_t *)(index)) + (sizeof(uint8_t) * 2))));
        Clock;
        set_color((uint8_t)((*((uint16_t *)(((uint8_t *)(index)) + (sizeof(uint8_t) * 3)))) >> (uint8_t)4));
        Clock;
        set_color((*(((uint8_t *)(index)) + (sizeof(uint8_t) * 5))));
        Clock;
        ++index;
        set_color((uint8_t)((*((uint16_t *)(index)) >> (uint8_t)4)));
        Clock;
        set_color((*(((uint8_t *)(index)) + (sizeof(uint8_t) * 2))));
        Clock;
        set_color((uint8_t)((*((uint16_t *)(((uint8_t *)(index)) + (sizeof(uint8_t) * 3)))) >> (uint8_t)4));
        Clock;
        set_color((*(((uint8_t *)(index)) + (sizeof(uint8_t) * 5))));
        Clock;

        ++index;
        set_color((uint8_t)((*((uint16_t *)(index)) >> (uint8_t)4)));
        Clock;
        set_color((*(((uint8_t *)(index)) + (sizeof(uint8_t) * 2))));
        Clock;
        set_color((uint8_t)((*((uint16_t *)(((uint8_t *)(index)) + (sizeof(uint8_t) * 3)))) >> (uint8_t)4));
        Clock;
        set_color((*(((uint8_t *)(index)) + (sizeof(uint8_t) * 5))));
        Clock;
        ++index;
        set_color((uint8_t)((*((uint16_t *)(index)) >> (uint8_t)4)));
        Clock;
        set_color((*(((uint8_t *)(index)) + (sizeof(uint8_t) * 2))));
        Clock;
        set_color((uint8_t)((*((uint16_t *)(((uint8_t *)(index)) + (sizeof(uint8_t) * 3)))) >> (uint8_t)4));
        Clock;
        set_color((*(((uint8_t *)(index)) + (sizeof(uint8_t) * 5))));
        Clock;

        ++index;
        set_color((uint8_t)((*((uint16_t *)(index)) >> (uint8_t)4)));
        Clock;
        set_color((*(((uint8_t *)(index)) + (sizeof(uint8_t) * 2))));
        Clock;
        set_color((uint8_t)((*((uint16_t *)(((uint8_t *)(index)) + (sizeof(uint8_t) * 3)))) >> (uint8_t)4));
        Clock;
        set_color((*(((uint8_t *)(index)) + (sizeof(uint8_t) * 5))));
        Clock;
        ++index;
        set_color((uint8_t)((*((uint16_t *)(index)) >> (uint8_t)4)));
        Clock;
        set_color((*(((uint8_t *)(index)) + (sizeof(uint8_t) * 2))));
        Clock;
        set_color((uint8_t)((*((uint16_t *)(((uint8_t *)(index)) + (sizeof(uint8_t) * 3)))) >> (uint8_t)4));
        Clock;
        set_color((*(((uint8_t *)(index)) + (sizeof(uint8_t) * 5))));
        Clock;

        ++index;
        set_color((uint8_t)((*((uint16_t *)(index)) >> (uint8_t)4)));
        Clock;
        set_color((*(((uint8_t *)(index)) + (sizeof(uint8_t) * 2))));
        Clock;
        set_color((uint8_t)((*((uint16_t *)(((uint8_t *)(index)) + (sizeof(uint8_t) * 3)))) >> (uint8_t)4));
        Clock;
        set_color((*(((uint8_t *)(index)) + (sizeof(uint8_t) * 5))));
        Clock;
        ++index;
        set_color((uint8_t)((*((uint16_t *)(index)) >> (uint8_t)4)));
        Clock;
        set_color((*(((uint8_t *)(index)) + (sizeof(uint8_t) * 2))));
        Clock;
        set_color((uint8_t)((*((uint16_t *)(((uint8_t *)(index)) + (sizeof(uint8_t) * 3)))) >> (uint8_t)4));
        Clock;
        set_color((*(((uint8_t *)(index)) + (sizeof(uint8_t) * 5))));
        Clock;

        ++index;
        set_color((uint8_t)((*((uint16_t *)(index)) >> (uint8_t)4)));
        Clock;
        set_color((*(((uint8_t *)(index)) + (sizeof(uint8_t) * 2))));
        Clock;
        set_color((uint8_t)((*((uint16_t *)(((uint8_t *)(index)) + (sizeof(uint8_t) * 3)))) >> (uint8_t)4));
        Clock;
        set_color((*(((uint8_t *)(index)) + (sizeof(uint8_t) * 5))));
        Clock;
        ++index;
        set_color((uint8_t)((*((uint16_t *)(index)) >> (uint8_t)4)));
        Clock;
        set_color((*(((uint8_t *)(index)) + (sizeof(uint8_t) * 2))));
        Clock;
        set_color((uint8_t)((*((uint16_t *)(((uint8_t *)(index)) + (sizeof(uint8_t) * 3)))) >> (uint8_t)4));
        Clock;
        set_color((*(((uint8_t *)(index)) + (sizeof(uint8_t) * 5))));
        Clock;

        ++index;
        set_color((uint8_t)((*((uint16_t *)(index)) >> (uint8_t)4)));
        Clock;
        set_color((*(((uint8_t *)(index)) + (sizeof(uint8_t) * 2))));
        Clock;
        set_color((uint8_t)((*((uint16_t *)(((uint8_t *)(index)) + (sizeof(uint8_t) * 3)))) >> (uint8_t)4));
        Clock;
        set_color((*(((uint8_t *)(index)) + (sizeof(uint8_t) * 5))));
        Clock;
        ++index;
        set_color((uint8_t)((*((uint16_t *)(index)) >> (uint8_t)4)));
        Clock;
        set_color((*(((uint8_t *)(index)) + (sizeof(uint8_t) * 2))));
        Clock;
        set_color((uint8_t)((*((uint16_t *)(((uint8_t *)(index)) + (sizeof(uint8_t) * 3)))) >> (uint8_t)4));
        Clock;
        set_color((*(((uint8_t *)(index)) + (sizeof(uint8_t) * 5))));
        Clock;

        ++index;
        set_color((uint8_t)((*((uint16_t *)(index)) >> (uint8_t)4)));
        Clock;
        set_color((*(((uint8_t *)(index)) + (sizeof(uint8_t) * 2))));
        Clock;
        set_color((uint8_t)((*((uint16_t *)(((uint8_t *)(index)) + (sizeof(uint8_t) * 3)))) >> (uint8_t)4));
        Clock;
        set_color((*(((uint8_t *)(index)) + (sizeof(uint8_t) * 5))));
        Clock;
        ++index;
        set_color((uint8_t)((*((uint16_t *)(index)) >> (uint8_t)4)));
        Clock;
        set_color((*(((uint8_t *)(index)) + (sizeof(uint8_t) * 2))));
        Clock;
        set_color((uint8_t)((*((uint16_t *)(((uint8_t *)(index)) + (sizeof(uint8_t) * 3)))) >> (uint8_t)4));
        Clock;
        set_color((*(((uint8_t *)(index)) + (sizeof(uint8_t) * 5))));
        Clock;

        ++index;
        set_color((uint8_t)((*((uint16_t *)(index)) >> (uint8_t)4)));
        Clock;
        set_color((*(((uint8_t *)(index)) + (sizeof(uint8_t) * 2))));
        Clock;
        set_color((uint8_t)((*((uint16_t *)(((uint8_t *)(index)) + (sizeof(uint8_t) * 3)))) >> (uint8_t)4));
        Clock;
        set_color((*(((uint8_t *)(index)) + (sizeof(uint8_t) * 5))));
        Clock;
        ++index;
        set_color((uint8_t)((*((uint16_t *)(index)) >> (uint8_t)4)));
        Clock;
        set_color((*(((uint8_t *)(index)) + (sizeof(uint8_t) * 2))));
        Clock;
        set_color((uint8_t)((*((uint16_t *)(((uint8_t *)(index)) + (sizeof(uint8_t) * 3)))) >> (uint8_t)4));
        Clock;
        set_color((*(((uint8_t *)(index)) + (sizeof(uint8_t) * 5))));
        Clock;
        // display row
        HIGH_OE;
        LATCH;
        stepRow();
        CLEAR_OE;

#if MAX_BRIGHTNESS_SLEEP_MUSEC > 0
        delayMicroseconds(BRIGHTNESS_SLEEP_MUSEC);
#if BRIGHTNESS_SLEEP_MUSEC < MAX_BRIGHTNESS_SLEEP_MUSEC

        delayMicroseconds(MAX_BRIGHTNESS_SLEEP_MUSEC - BRIGHTNESS_SLEEP_MUSEC);
        HIGH_OE;
#endif
#endif
    }
    // lsb
    for (uint8_t y = 0; y < PANEL_Y / 2; y++)
    {

        index = (LED *)(&buffer) + (y << (uint8_t)4);

        set_color(*(uint8_t *)(index) << (uint8_t)2);
        Clock;
        set_color((uint8_t)((*((uint16_t *)(((uint8_t *)(index)) + sizeof(uint8_t)))) >> (uint8_t)2));
        Clock;
        set_color((*(((uint8_t *)(index)) + (sizeof(uint8_t) * 3))) << (uint8_t)2);
        Clock;
        set_color((uint8_t)((*((uint16_t *)(((uint8_t *)(index)) + (sizeof(uint8_t) * 4)))) >> (uint8_t)2));
        Clock;
        ++index;
        set_color(*(uint8_t *)(index) << (uint8_t)2);
        Clock;
        set_color((uint8_t)((*((uint16_t *)(((uint8_t *)(index)) + sizeof(uint8_t)))) >> (uint8_t)2));
        Clock;
        set_color((*(((uint8_t *)(index)) + (sizeof(uint8_t) * 3))) << (uint8_t)2);
        Clock;
        set_color((uint8_t)((*((uint16_t *)(((uint8_t *)(index)) + (sizeof(uint8_t) * 4)))) >> (uint8_t)2));
        Clock;

        ++index;
        set_color(*(uint8_t *)(index) << (uint8_t)2);
        Clock;
        set_color((uint8_t)((*((uint16_t *)(((uint8_t *)(index)) + sizeof(uint8_t)))) >> (uint8_t)2));
        Clock;
        set_color((*(((uint8_t *)(index)) + (sizeof(uint8_t) * 3))) << (uint8_t)2);
        Clock;
        set_color((uint8_t)((*((uint16_t *)(((uint8_t *)(index)) + (sizeof(uint8_t) * 4)))) >> (uint8_t)2));
        Clock;
        ++index;
        set_color(*(uint8_t *)(index) << (uint8_t)2);
        Clock;
        set_color((uint8_t)((*((uint16_t *)(((uint8_t *)(index)) + sizeof(uint8_t)))) >> (uint8_t)2));
        Clock;
        set_color((*(((uint8_t *)(index)) + (sizeof(uint8_t) * 3))) << (uint8_t)2);
        Clock;
        set_color((uint8_t)((*((uint16_t *)(((uint8_t *)(index)) + (sizeof(uint8_t) * 4)))) >> (uint8_t)2));
        Clock;

        ++index;
        set_color(*(uint8_t *)(index) << (uint8_t)2);
        Clock;
        set_color((uint8_t)((*((uint16_t *)(((uint8_t *)(index)) + sizeof(uint8_t)))) >> (uint8_t)2));
        Clock;
        set_color((*(((uint8_t *)(index)) + (sizeof(uint8_t) * 3))) << (uint8_t)2);
        Clock;
        set_color((uint8_t)((*((uint16_t *)(((uint8_t *)(index)) + (sizeof(uint8_t) * 4)))) >> (uint8_t)2));
        Clock;
        ++index;
        set_color(*(uint8_t *)(index) << (uint8_t)2);
        Clock;
        set_color((uint8_t)((*((uint16_t *)(((uint8_t *)(index)) + sizeof(uint8_t)))) >> (uint8_t)2));
        Clock;
        set_color((*(((uint8_t *)(index)) + (sizeof(uint8_t) * 3))) << (uint8_t)2);
        Clock;
        set_color((uint8_t)((*((uint16_t *)(((uint8_t *)(index)) + (sizeof(uint8_t) * 4)))) >> (uint8_t)2));
        Clock;

        ++index;
        set_color(*(uint8_t *)(index) << (uint8_t)2);
        Clock;
        set_color((uint8_t)((*((uint16_t *)(((uint8_t *)(index)) + sizeof(uint8_t)))) >> (uint8_t)2));
        Clock;
        set_color((*(((uint8_t *)(index)) + (sizeof(uint8_t) * 3))) << (uint8_t)2);
        Clock;
        set_color((uint8_t)((*((uint16_t *)(((uint8_t *)(index)) + (sizeof(uint8_t) * 4)))) >> (uint8_t)2));
        Clock;
        ++index;
        set_color(*(uint8_t *)(index) << (uint8_t)2);
        Clock;
        set_color((uint8_t)((*((uint16_t *)(((uint8_t *)(index)) + sizeof(uint8_t)))) >> (uint8_t)2));
        Clock;
        set_color((*(((uint8_t *)(index)) + (sizeof(uint8_t) * 3))) << (uint8_t)2);
        Clock;
        set_color((uint8_t)((*((uint16_t *)(((uint8_t *)(index)) + (sizeof(uint8_t) * 4)))) >> (uint8_t)2));
        Clock;

        ++index;
        set_color(*(uint8_t *)(index) << (uint8_t)2);
        Clock;
        set_color((uint8_t)((*((uint16_t *)(((uint8_t *)(index)) + sizeof(uint8_t)))) >> (uint8_t)2));
        Clock;
        set_color((*(((uint8_t *)(index)) + (sizeof(uint8_t) * 3))) << (uint8_t)2);
        Clock;
        set_color((uint8_t)((*((uint16_t *)(((uint8_t *)(index)) + (sizeof(uint8_t) * 4)))) >> (uint8_t)2));
        Clock;
        ++index;
        set_color(*(uint8_t *)(index) << (uint8_t)2);
        Clock;
        set_color((uint8_t)((*((uint16_t *)(((uint8_t *)(index)) + sizeof(uint8_t)))) >> (uint8_t)2));
        Clock;
        set_color((*(((uint8_t *)(index)) + (sizeof(uint8_t) * 3))) << (uint8_t)2);
        Clock;
        set_color((uint8_t)((*((uint16_t *)(((uint8_t *)(index)) + (sizeof(uint8_t) * 4)))) >> (uint8_t)2));
        Clock;

        ++index;
        set_color(*(uint8_t *)(index) << (uint8_t)2);
        Clock;
        set_color((uint8_t)((*((uint16_t *)(((uint8_t *)(index)) + sizeof(uint8_t)))) >> (uint8_t)2));
        Clock;
        set_color((*(((uint8_t *)(index)) + (sizeof(uint8_t) * 3))) << (uint8_t)2);
        Clock;
        set_color((uint8_t)((*((uint16_t *)(((uint8_t *)(index)) + (sizeof(uint8_t) * 4)))) >> (uint8_t)2));
        Clock;
        ++index;
        set_color(*(uint8_t *)(index) << (uint8_t)2);
        Clock;
        set_color((uint8_t)((*((uint16_t *)(((uint8_t *)(index)) + sizeof(uint8_t)))) >> (uint8_t)2));
        Clock;
        set_color((*(((uint8_t *)(index)) + (sizeof(uint8_t) * 3))) << (uint8_t)2);
        Clock;
        set_color((uint8_t)((*((uint16_t *)(((uint8_t *)(index)) + (sizeof(uint8_t) * 4)))) >> (uint8_t)2));
        Clock;

        ++index;
        set_color(*(uint8_t *)(index) << (uint8_t)2);
        Clock;
        set_color((uint8_t)((*((uint16_t *)(((uint8_t *)(index)) + sizeof(uint8_t)))) >> (uint8_t)2));
        Clock;
        set_color((*(((uint8_t *)(index)) + (sizeof(uint8_t) * 3))) << (uint8_t)2);
        Clock;
        set_color((uint8_t)((*((uint16_t *)(((uint8_t *)(index)) + (sizeof(uint8_t) * 4)))) >> (uint8_t)2));
        Clock;
        ++index;
        set_color(*(uint8_t *)(index) << (uint8_t)2);
        Clock;
        set_color((uint8_t)((*((uint16_t *)(((uint8_t *)(index)) + sizeof(uint8_t)))) >> (uint8_t)2));
        Clock;
        set_color((*(((uint8_t *)(index)) + (sizeof(uint8_t) * 3))) << (uint8_t)2);
        Clock;
        set_color((uint8_t)((*((uint16_t *)(((uint8_t *)(index)) + (sizeof(uint8_t) * 4)))) >> (uint8_t)2));
        Clock;

        ++index;
        set_color(*(uint8_t *)(index) << (uint8_t)2);
        Clock;
        set_color((uint8_t)((*((uint16_t *)(((uint8_t *)(index)) + sizeof(uint8_t)))) >> (uint8_t)2));
        Clock;
        set_color((*(((uint8_t *)(index)) + (sizeof(uint8_t) * 3))) << (uint8_t)2);
        Clock;
        set_color((uint8_t)((*((uint16_t *)(((uint8_t *)(index)) + (sizeof(uint8_t) * 4)))) >> (uint8_t)2));
        Clock;
        ++index;
        set_color(*(uint8_t *)(index) << (uint8_t)2);
        Clock;
        set_color((uint8_t)((*((uint16_t *)(((uint8_t *)(index)) + sizeof(uint8_t)))) >> (uint8_t)2));
        Clock;
        set_color((*(((uint8_t *)(index)) + (sizeof(uint8_t) * 3))) << (uint8_t)2);
        Clock;
        set_color((uint8_t)((*((uint16_t *)(((uint8_t *)(index)) + (sizeof(uint8_t) * 4)))) >> (uint8_t)2));
        Clock;
        // display row
        HIGH_OE;
        LATCH;
        stepRow();
        CLEAR_OE;

#if MAX_BRIGHTNESS_SLEEP_MUSEC > 0
        delayMicroseconds(BRIGHTNESS_SLEEP_MUSEC / 2);
#if BRIGHTNESS_SLEEP_MUSEC < MAX_BRIGHTNESS_SLEEP_MUSEC

        delayMicroseconds(MAX_BRIGHTNESS_SLEEP_MUSEC / 2 - BRIGHTNESS_SLEEP_MUSEC / 2);
        HIGH_OE;

#endif
#endif
    }
    HIGH_OE;
}

void setBigBuffer(uint8_t x, uint8_t y, Color color)
{
#ifdef PANEL_FLIP_VERTICAL
    y = PANEL_Y - y;
#endif
#ifdef PANEL_FLIP_HORIZONTAL
    x = PANEL_X - x;
#endif
    if (y < (PANEL_Y / 2))
    {
        // we are in upper half of pixels
        uint8_t index = (y * PANEL_X + x) / 4;
        switch (x & 3)
        {
        case 0: /*first pixel*/
            buffer[index].redUpperBit1Led1 = color.red;
            buffer[index].greenUpperBit1Led1 = color.green;
            buffer[index].blueUpperBit1Led1 = color.blue;
            buffer[index].redUpperBit2Led1 = color.red >> (uint8_t)1;
            buffer[index].greenUpperBit2Led1 = color.green >> (uint8_t)1;
            buffer[index].blueUpperBit2Led1 = color.blue >> (uint8_t)1;
            break;
        case 1: /*second pixel*/
            buffer[index].redUpperBit1Led2 = color.red;
            buffer[index].greenUpperBit1Led2 = color.green;
            buffer[index].blueUpperBit1Led2 = color.blue;
            buffer[index].redUpperBit2Led2 = color.red >> (uint8_t)1;
            buffer[index].greenUpperBit2Led2 = color.green >> (uint8_t)1;
            buffer[index].blueUpperBit2Led2 = color.blue >> (uint8_t)1;
            break;
        case 2: /*third pixel*/
            buffer[index].redUpperBit1Led3 = color.red;
            buffer[index].greenUpperBit1Led3 = color.green;
            buffer[index].blueUpperBit1Led3 = color.blue;
            buffer[index].redUpperBit2Led3 = color.red >> (uint8_t)1;
            buffer[index].greenUpperBit2Led3 = color.green >> (uint8_t)1;
            buffer[index].blueUpperBit2Led3 = color.blue >> (uint8_t)1;
            break;
        case 3: /*fourth pixel*/
            buffer[index].redUpperBit1Led4 = color.red;
            buffer[index].greenUpperBit1Led4 = color.green;
            buffer[index].blueUpperBit1Led4 = color.blue;
            buffer[index].redUpperBit2Led4 = color.red >> (uint8_t)1;
            buffer[index].greenUpperBit2Led4 = color.green >> (uint8_t)1;
            buffer[index].blueUpperBit2Led4 = color.blue >> (uint8_t)1;
            break;
        default:
            break;
        }
    }
    else
    {
        y -= (PANEL_Y / 2);
        // we are in lower half of pixels
        uint8_t index = (y * PANEL_X + x) / 4;
        switch (x & 3)
        {
        case 0:
            buffer[index].redLowerBit1Led1 = color.red;
            buffer[index].greenLowerBit1Led1 = color.green;
            buffer[index].blueLowerBit1Led1 = color.blue;
            buffer[index].redLowerBit2Led1 = color.red >> (uint8_t)1;
            buffer[index].greenLowerBit2Led1 = color.green >> (uint8_t)1;
            buffer[index].blueLowerBit2Led1 = color.blue >> (uint8_t)1;
            break;
        case 1: /*second pixel*/
            buffer[index].redLowerBit1Led2 = color.red;
            buffer[index].greenLowerBit1Led2 = color.green;
            buffer[index].blueLowerBit1Led2 = color.blue;
            buffer[index].redLowerBit2Led2 = color.red >> (uint8_t)1;
            buffer[index].greenLowerBit2Led2 = color.green >> (uint8_t)1;
            buffer[index].blueLowerBit2Led2 = color.blue >> (uint8_t)1;
            break;
        case 2: /*third pixel*/
            buffer[index].redLowerBit1Led3 = color.red;
            buffer[index].greenLowerBit1Led3 = color.green;
            buffer[index].blueLowerBit1Led3 = color.blue;
            buffer[index].redLowerBit2Led3 = color.red >> (uint8_t)1;
            buffer[index].greenLowerBit2Led3 = color.green >> (uint8_t)1;
            buffer[index].blueLowerBit2Led3 = color.blue >> (uint8_t)1;
            break;
        case 3: /*fourth pixel*/
            buffer[index].redLowerBit1Led4 = color.red;
            buffer[index].greenLowerBit1Led4 = color.green;
            buffer[index].blueLowerBit1Led4 = color.blue;
            buffer[index].redLowerBit2Led4 = color.red >> (uint8_t)1;
            buffer[index].greenLowerBit2Led4 = color.green >> (uint8_t)1;
            buffer[index].blueLowerBit2Led4 = color.blue >> (uint8_t)1;
            break;
        default:
            break;
        }
    }
}

void setBigBuffer4x(uint8_t x, uint8_t y, uint8_t block_count, Color color)
{
#ifdef PANEL_FLIP_VERTICAL
    y = PANEL_Y - y;
#endif
#ifdef PANEL_FLIP_HORIZONTAL
    x = PANEL_X - x;
#endif
    if (y < (PANEL_Y / 2))
    {
        // we are in upper half of pixels
        uint16_t index = ((y * PANEL_X) + x) / 4;

        buffer[index].redUpperBit1Led1 = color.red;
        buffer[index].greenUpperBit1Led1 = color.green;
        buffer[index].blueUpperBit1Led1 = color.blue;
        buffer[index].redUpperBit1Led2 = color.red;
        buffer[index].greenUpperBit1Led2 = color.green;
        buffer[index].blueUpperBit1Led2 = color.blue;
        buffer[index].redUpperBit1Led3 = color.red;
        buffer[index].greenUpperBit1Led3 = color.green;
        buffer[index].blueUpperBit1Led3 = color.blue;
        buffer[index].redUpperBit1Led4 = color.red;
        buffer[index].greenUpperBit1Led4 = color.green;
        buffer[index].blueUpperBit1Led4 = color.blue;
        // second bit
        buffer[index].redUpperBit2Led1 = color.red >> (uint8_t)1;
        buffer[index].greenUpperBit2Led1 = color.green >> (uint8_t)1;
        buffer[index].blueUpperBit2Led1 = color.blue >> (uint8_t)1;
        buffer[index].redUpperBit2Led2 = color.red >> (uint8_t)1;
        buffer[index].greenUpperBit2Led2 = color.green >> (uint8_t)1;
        buffer[index].blueUpperBit2Led2 = color.blue >> (uint8_t)1;
        buffer[index].redUpperBit2Led3 = color.red >> (uint8_t)1;
        buffer[index].greenUpperBit2Led3 = color.green >> (uint8_t)1;
        buffer[index].blueUpperBit2Led3 = color.blue >> (uint8_t)1;
        buffer[index].redUpperBit2Led4 = color.red >> (uint8_t)1;
        buffer[index].greenUpperBit2Led4 = color.green >> (uint8_t)1;
        buffer[index].blueUpperBit2Led4 = color.blue >> (uint8_t)1;

        // temp buffers to store the cleaned values
        uint8_t lowest, lower, lowmid, mid, higher, highest;
        uint8_t *start = (uint8_t *)&buffer[index];

        // clean and cache the bits needed for thsi half
        lowest = ((*start) & LED_LONG_MASK_UPPER_1);
        start++;
        lower = ((*start) & LED_LONG_MASK_UPPER_2);
        start++;
        lowmid = ((*start) & LED_LONG_MASK_UPPER_3);
        start++;
        mid = ((*start) & LED_LONG_MASK_UPPER_4);
        start++;
        higher = ((*start) & LED_LONG_MASK_UPPER_5);
        start++;
        highest = ((*start) & LED_LONG_MASK_UPPER_6);
        start++;

        for (uint8_t i = 1; i < block_count; i++)
        {
            // apply the masks so we only copy the one half of the pixels we are "allowed" to
            *start &= LED_LONG_MASK_LOWER_1;
            *start |= lowest;
            start++;
            *start &= LED_LONG_MASK_LOWER_2;
            *start |= lower;
            start++;
            *start &= LED_LONG_MASK_LOWER_3;
            *start |= lowmid;
            start++;
            *start &= LED_LONG_MASK_LOWER_4;
            *start |= mid;
            start++;
            *start &= LED_LONG_MASK_LOWER_5;
            *start |= higher;
            start++;
            *start &= LED_LONG_MASK_LOWER_6;
            *start |= highest;
            start++;
        }
    }
    else
    {
        y -= (PANEL_Y / 2);
        // we are in lower half of pixels
        uint16_t index = ((y * PANEL_X) + x) / 4;

        buffer[index].redLowerBit1Led1 = color.red;
        buffer[index].greenLowerBit1Led1 = color.green;
        buffer[index].blueLowerBit1Led1 = color.blue;
        buffer[index].redLowerBit1Led2 = color.red;
        buffer[index].greenLowerBit1Led2 = color.green;
        buffer[index].blueLowerBit1Led2 = color.blue;
        buffer[index].redLowerBit1Led3 = color.red;
        buffer[index].greenLowerBit1Led3 = color.green;
        buffer[index].blueLowerBit1Led3 = color.blue;
        buffer[index].redLowerBit1Led4 = color.red;
        buffer[index].greenLowerBit1Led4 = color.green;
        buffer[index].blueLowerBit1Led4 = color.blue;
        // second bit
        buffer[index].redLowerBit2Led1 = color.red >> (uint8_t)1;
        buffer[index].greenLowerBit2Led1 = color.green >> (uint8_t)1;
        buffer[index].blueLowerBit2Led1 = color.blue >> (uint8_t)1;
        buffer[index].redLowerBit2Led2 = color.red >> (uint8_t)1;
        buffer[index].greenLowerBit2Led2 = color.green >> (uint8_t)1;
        buffer[index].blueLowerBit2Led2 = color.blue >> (uint8_t)1;
        buffer[index].redLowerBit2Led3 = color.red >> (uint8_t)1;
        buffer[index].greenLowerBit2Led3 = color.green >> (uint8_t)1;
        buffer[index].blueLowerBit2Led3 = color.blue >> (uint8_t)1;
        buffer[index].redLowerBit2Led4 = color.red >> (uint8_t)1;
        buffer[index].greenLowerBit2Led4 = color.green >> (uint8_t)1;
        buffer[index].blueLowerBit2Led4 = color.blue >> (uint8_t)1;

        // temp buffers to store the cleaned values
        uint8_t lowest, lower, lowmid, mid, higher, highest;
        uint8_t *start = (uint8_t *)&buffer[index];

        // clean and cache the bits needed for thsi half
        lowest = ((*start) & LED_LONG_MASK_LOWER_1);
        start++;
        lower = ((*start) & LED_LONG_MASK_LOWER_2);
        start++;
        lowmid = ((*start) & LED_LONG_MASK_LOWER_3);
        start++;
        mid = ((*start) & LED_LONG_MASK_LOWER_4);
        start++;
        higher = ((*start) & LED_LONG_MASK_LOWER_5);
        start++;
        highest = ((*start) & LED_LONG_MASK_LOWER_6);
        start++;

        for (uint8_t i = 1; i < block_count; i++)
        {
            // apply the masks so we only copy the one half of the pixels we are "allowed" to
            *start &= LED_LONG_MASK_UPPER_1;
            *start |= lowest;
            start++;
            *start &= LED_LONG_MASK_UPPER_2;
            *start |= lower;
            start++;
            *start &= LED_LONG_MASK_UPPER_3;
            *start |= lowmid;
            start++;
            *start &= LED_LONG_MASK_UPPER_4;
            *start |= mid;
            start++;
            *start &= LED_LONG_MASK_UPPER_5;
            *start |= higher;
            start++;
            *start &= LED_LONG_MASK_UPPER_6;
            *start |= highest;
            start++;
        }
    }
}
#endif
#else

    void displayFlashBuffer()
    {
        uint16_t index = 0;
#ifdef PANEL_FLIP_HORIZONTAL
#define INDEX_MOVE index--
#else
#define INDEX_MOVE index++
#endif
        for (uint8_t y = 0; y < PANEL_Y; y++) // 32 rows
        {
            // we send first the MMSB, then MSB, LSB, LLSB
            index = (uint16_t)(buffer + (y << (uint8_t)6));
#ifdef PANEL_FLIP_HORIZONTAL
            index += PANEL_X;
#endif

#pragma region MMSB
            set_color(pgm_read_byte(INDEX_MOVE));
            Clock;
            set_color(pgm_read_byte(INDEX_MOVE));
            Clock;
            set_color(pgm_read_byte(INDEX_MOVE));
            Clock;
            set_color(pgm_read_byte(INDEX_MOVE));
            Clock;
            set_color(pgm_read_byte(INDEX_MOVE));
            Clock;
            set_color(pgm_read_byte(INDEX_MOVE));
            Clock;
            set_color(pgm_read_byte(INDEX_MOVE));
            Clock;
            set_color(pgm_read_byte(INDEX_MOVE));
            Clock;

            set_color(pgm_read_byte(INDEX_MOVE));
            Clock;
            set_color(pgm_read_byte(INDEX_MOVE));
            Clock;
            set_color(pgm_read_byte(INDEX_MOVE));
            Clock;
            set_color(pgm_read_byte(INDEX_MOVE));
            Clock;
            set_color(pgm_read_byte(INDEX_MOVE));
            Clock;
            set_color(pgm_read_byte(INDEX_MOVE));
            Clock;
            set_color(pgm_read_byte(INDEX_MOVE));
            Clock;
            set_color(pgm_read_byte(INDEX_MOVE));
            Clock;

            set_color(pgm_read_byte(INDEX_MOVE));
            Clock;
            set_color(pgm_read_byte(INDEX_MOVE));
            Clock;
            set_color(pgm_read_byte(INDEX_MOVE));
            Clock;
            set_color(pgm_read_byte(INDEX_MOVE));
            Clock;
            set_color(pgm_read_byte(INDEX_MOVE));
            Clock;
            set_color(pgm_read_byte(INDEX_MOVE));
            Clock;
            set_color(pgm_read_byte(INDEX_MOVE));
            Clock;
            set_color(pgm_read_byte(INDEX_MOVE));
            Clock;

            set_color(pgm_read_byte(INDEX_MOVE));
            Clock;
            set_color(pgm_read_byte(INDEX_MOVE));
            Clock;
            set_color(pgm_read_byte(INDEX_MOVE));
            Clock;
            set_color(pgm_read_byte(INDEX_MOVE));
            Clock;
            set_color(pgm_read_byte(INDEX_MOVE));
            Clock;
            set_color(pgm_read_byte(INDEX_MOVE));
            Clock;
            set_color(pgm_read_byte(INDEX_MOVE));
            Clock;
            set_color(pgm_read_byte(INDEX_MOVE));
            Clock;

            set_color(pgm_read_byte(INDEX_MOVE));
            Clock;
            set_color(pgm_read_byte(INDEX_MOVE));
            Clock;
            set_color(pgm_read_byte(INDEX_MOVE));
            Clock;
            set_color(pgm_read_byte(INDEX_MOVE));
            Clock;
            set_color(pgm_read_byte(INDEX_MOVE));
            Clock;
            set_color(pgm_read_byte(INDEX_MOVE));
            Clock;
            set_color(pgm_read_byte(INDEX_MOVE));
            Clock;
            set_color(pgm_read_byte(INDEX_MOVE));
            Clock;

            set_color(pgm_read_byte(INDEX_MOVE));
            Clock;
            set_color(pgm_read_byte(INDEX_MOVE));
            Clock;
            set_color(pgm_read_byte(INDEX_MOVE));
            Clock;
            set_color(pgm_read_byte(INDEX_MOVE));
            Clock;
            set_color(pgm_read_byte(INDEX_MOVE));
            Clock;
            set_color(pgm_read_byte(INDEX_MOVE));
            Clock;
            set_color(pgm_read_byte(INDEX_MOVE));
            Clock;
            set_color(pgm_read_byte(INDEX_MOVE));
            Clock;

            set_color(pgm_read_byte(INDEX_MOVE));
            Clock;
            set_color(pgm_read_byte(INDEX_MOVE));
            Clock;
            set_color(pgm_read_byte(INDEX_MOVE));
            Clock;
            set_color(pgm_read_byte(INDEX_MOVE));
            Clock;
            set_color(pgm_read_byte(INDEX_MOVE));
            Clock;
            set_color(pgm_read_byte(INDEX_MOVE));
            Clock;
            set_color(pgm_read_byte(INDEX_MOVE));
            Clock;
            set_color(pgm_read_byte(INDEX_MOVE));
            Clock;

            set_color(pgm_read_byte(INDEX_MOVE));
            Clock;
            set_color(pgm_read_byte(INDEX_MOVE));
            Clock;
            set_color(pgm_read_byte(INDEX_MOVE));
            Clock;
            set_color(pgm_read_byte(INDEX_MOVE));
            Clock;
            set_color(pgm_read_byte(INDEX_MOVE));
            Clock;
            set_color(pgm_read_byte(INDEX_MOVE));
            Clock;
            set_color(pgm_read_byte(INDEX_MOVE));
            Clock;
            set_color(pgm_read_byte(index));
            Clock;
            // shift data into buffers
            HIGH_OE;
            LATCH;
            stepRow();
            CLEAR_OE;
#if MAX_BRIGHTNESS_SLEEP_MUSEC > 0
            delayMicroseconds(BRIGHTNESS_SLEEP_MUSEC);
#if BRIGHTNESS_SLEEP_MUSEC < MAX_BRIGHTNESS_SLEEP_MUSEC

            HIGH_OE;
            delayMicroseconds(MAX_BRIGHTNESS_SLEEP_MUSEC - BRIGHTNESS_SLEEP_MUSEC);
#endif
#endif
        }
#pragma endregion // MMSB

        for (uint8_t y = 0; y < PANEL_Y; y++) // 32 rows
        {
            index = (uint16_t)(buffer + (y << (uint8_t)6)) + (PANEL_BUFFERSIZE / 4);

#ifdef PANEL_FLIP_HORIZONTAL
            index += PANEL_X;
#endif
#pragma region MSB
            set_color(pgm_read_byte(INDEX_MOVE));
            Clock;
            set_color(pgm_read_byte(INDEX_MOVE));
            Clock;
            set_color(pgm_read_byte(INDEX_MOVE));
            Clock;
            set_color(pgm_read_byte(INDEX_MOVE));
            Clock;
            set_color(pgm_read_byte(INDEX_MOVE));
            Clock;
            set_color(pgm_read_byte(INDEX_MOVE));
            Clock;
            set_color(pgm_read_byte(INDEX_MOVE));
            Clock;
            set_color(pgm_read_byte(INDEX_MOVE));
            Clock;

            set_color(pgm_read_byte(INDEX_MOVE));
            Clock;
            set_color(pgm_read_byte(INDEX_MOVE));
            Clock;
            set_color(pgm_read_byte(INDEX_MOVE));
            Clock;
            set_color(pgm_read_byte(INDEX_MOVE));
            Clock;
            set_color(pgm_read_byte(INDEX_MOVE));
            Clock;
            set_color(pgm_read_byte(INDEX_MOVE));
            Clock;
            set_color(pgm_read_byte(INDEX_MOVE));
            Clock;
            set_color(pgm_read_byte(INDEX_MOVE));
            Clock;

            set_color(pgm_read_byte(INDEX_MOVE));
            Clock;
            set_color(pgm_read_byte(INDEX_MOVE));
            Clock;
            set_color(pgm_read_byte(INDEX_MOVE));
            Clock;
            set_color(pgm_read_byte(INDEX_MOVE));
            Clock;
            set_color(pgm_read_byte(INDEX_MOVE));
            Clock;
            set_color(pgm_read_byte(INDEX_MOVE));
            Clock;
            set_color(pgm_read_byte(INDEX_MOVE));
            Clock;
            set_color(pgm_read_byte(INDEX_MOVE));
            Clock;

            set_color(pgm_read_byte(INDEX_MOVE));
            Clock;
            set_color(pgm_read_byte(INDEX_MOVE));
            Clock;
            set_color(pgm_read_byte(INDEX_MOVE));
            Clock;
            set_color(pgm_read_byte(INDEX_MOVE));
            Clock;
            set_color(pgm_read_byte(INDEX_MOVE));
            Clock;
            set_color(pgm_read_byte(INDEX_MOVE));
            Clock;
            set_color(pgm_read_byte(INDEX_MOVE));
            Clock;
            set_color(pgm_read_byte(INDEX_MOVE));
            Clock;

            set_color(pgm_read_byte(INDEX_MOVE));
            Clock;
            set_color(pgm_read_byte(INDEX_MOVE));
            Clock;
            set_color(pgm_read_byte(INDEX_MOVE));
            Clock;
            set_color(pgm_read_byte(INDEX_MOVE));
            Clock;
            set_color(pgm_read_byte(INDEX_MOVE));
            Clock;
            set_color(pgm_read_byte(INDEX_MOVE));
            Clock;
            set_color(pgm_read_byte(INDEX_MOVE));
            Clock;
            set_color(pgm_read_byte(INDEX_MOVE));
            Clock;

            set_color(pgm_read_byte(INDEX_MOVE));
            Clock;
            set_color(pgm_read_byte(INDEX_MOVE));
            Clock;
            set_color(pgm_read_byte(INDEX_MOVE));
            Clock;
            set_color(pgm_read_byte(INDEX_MOVE));
            Clock;
            set_color(pgm_read_byte(INDEX_MOVE));
            Clock;
            set_color(pgm_read_byte(INDEX_MOVE));
            Clock;
            set_color(pgm_read_byte(INDEX_MOVE));
            Clock;
            set_color(pgm_read_byte(INDEX_MOVE));
            Clock;

            set_color(pgm_read_byte(INDEX_MOVE));
            Clock;
            set_color(pgm_read_byte(INDEX_MOVE));
            Clock;
            set_color(pgm_read_byte(INDEX_MOVE));
            Clock;
            set_color(pgm_read_byte(INDEX_MOVE));
            Clock;
            set_color(pgm_read_byte(INDEX_MOVE));
            Clock;
            set_color(pgm_read_byte(INDEX_MOVE));
            Clock;
            set_color(pgm_read_byte(INDEX_MOVE));
            Clock;
            set_color(pgm_read_byte(INDEX_MOVE));
            Clock;

            set_color(pgm_read_byte(INDEX_MOVE));
            Clock;
            set_color(pgm_read_byte(INDEX_MOVE));
            Clock;
            set_color(pgm_read_byte(INDEX_MOVE));
            Clock;
            set_color(pgm_read_byte(INDEX_MOVE));
            Clock;
            set_color(pgm_read_byte(INDEX_MOVE));
            Clock;
            set_color(pgm_read_byte(INDEX_MOVE));
            Clock;
            set_color(pgm_read_byte(INDEX_MOVE));
            Clock;
            set_color(pgm_read_byte(index));
            Clock;
            // shift data into buffers
            HIGH_OE;
            LATCH;
            stepRow();
            CLEAR_OE;
#if MAX_BRIGHTNESS_SLEEP_MUSEC > 0
            delayMicroseconds(BRIGHTNESS_SLEEP_MUSEC / 2);
#if BRIGHTNESS_SLEEP_MUSEC < MAX_BRIGHTNESS_SLEEP_MUSEC

            HIGH_OE;
            delayMicroseconds(MAX_BRIGHTNESS_SLEEP_MUSEC / 2 - BRIGHTNESS_SLEEP_MUSEC / 2);

#endif
#endif
        }
#pragma endregion // MMSB

        for (uint8_t y = 0; y < PANEL_Y; y++) // 32 rows
        {
            index = (uint16_t)(buffer + (y << (uint8_t)6)) + (PANEL_BUFFERSIZE / 2);

#ifdef PANEL_FLIP_HORIZONTAL
            index += PANEL_X;
#endif
#pragma region LSB
            set_color(pgm_read_byte(INDEX_MOVE));
            Clock;
            set_color(pgm_read_byte(INDEX_MOVE));
            Clock;
            set_color(pgm_read_byte(INDEX_MOVE));
            Clock;
            set_color(pgm_read_byte(INDEX_MOVE));
            Clock;
            set_color(pgm_read_byte(INDEX_MOVE));
            Clock;
            set_color(pgm_read_byte(INDEX_MOVE));
            Clock;
            set_color(pgm_read_byte(INDEX_MOVE));
            Clock;
            set_color(pgm_read_byte(INDEX_MOVE));
            Clock;

            set_color(pgm_read_byte(INDEX_MOVE));
            Clock;
            set_color(pgm_read_byte(INDEX_MOVE));
            Clock;
            set_color(pgm_read_byte(INDEX_MOVE));
            Clock;
            set_color(pgm_read_byte(INDEX_MOVE));
            Clock;
            set_color(pgm_read_byte(INDEX_MOVE));
            Clock;
            set_color(pgm_read_byte(INDEX_MOVE));
            Clock;
            set_color(pgm_read_byte(INDEX_MOVE));
            Clock;
            set_color(pgm_read_byte(INDEX_MOVE));
            Clock;

            set_color(pgm_read_byte(INDEX_MOVE));
            Clock;
            set_color(pgm_read_byte(INDEX_MOVE));
            Clock;
            set_color(pgm_read_byte(INDEX_MOVE));
            Clock;
            set_color(pgm_read_byte(INDEX_MOVE));
            Clock;
            set_color(pgm_read_byte(INDEX_MOVE));
            Clock;
            set_color(pgm_read_byte(INDEX_MOVE));
            Clock;
            set_color(pgm_read_byte(INDEX_MOVE));
            Clock;
            set_color(pgm_read_byte(INDEX_MOVE));
            Clock;

            set_color(pgm_read_byte(INDEX_MOVE));
            Clock;
            set_color(pgm_read_byte(INDEX_MOVE));
            Clock;
            set_color(pgm_read_byte(INDEX_MOVE));
            Clock;
            set_color(pgm_read_byte(INDEX_MOVE));
            Clock;
            set_color(pgm_read_byte(INDEX_MOVE));
            Clock;
            set_color(pgm_read_byte(INDEX_MOVE));
            Clock;
            set_color(pgm_read_byte(INDEX_MOVE));
            Clock;
            set_color(pgm_read_byte(INDEX_MOVE));
            Clock;

            set_color(pgm_read_byte(INDEX_MOVE));
            Clock;
            set_color(pgm_read_byte(INDEX_MOVE));
            Clock;
            set_color(pgm_read_byte(INDEX_MOVE));
            Clock;
            set_color(pgm_read_byte(INDEX_MOVE));
            Clock;
            set_color(pgm_read_byte(INDEX_MOVE));
            Clock;
            set_color(pgm_read_byte(INDEX_MOVE));
            Clock;
            set_color(pgm_read_byte(INDEX_MOVE));
            Clock;
            set_color(pgm_read_byte(INDEX_MOVE));
            Clock;

            set_color(pgm_read_byte(INDEX_MOVE));
            Clock;
            set_color(pgm_read_byte(INDEX_MOVE));
            Clock;
            set_color(pgm_read_byte(INDEX_MOVE));
            Clock;
            set_color(pgm_read_byte(INDEX_MOVE));
            Clock;
            set_color(pgm_read_byte(INDEX_MOVE));
            Clock;
            set_color(pgm_read_byte(INDEX_MOVE));
            Clock;
            set_color(pgm_read_byte(INDEX_MOVE));
            Clock;
            set_color(pgm_read_byte(INDEX_MOVE));
            Clock;

            set_color(pgm_read_byte(INDEX_MOVE));
            Clock;
            set_color(pgm_read_byte(INDEX_MOVE));
            Clock;
            set_color(pgm_read_byte(INDEX_MOVE));
            Clock;
            set_color(pgm_read_byte(INDEX_MOVE));
            Clock;
            set_color(pgm_read_byte(INDEX_MOVE));
            Clock;
            set_color(pgm_read_byte(INDEX_MOVE));
            Clock;
            set_color(pgm_read_byte(INDEX_MOVE));
            Clock;
            set_color(pgm_read_byte(INDEX_MOVE));
            Clock;

            set_color(pgm_read_byte(INDEX_MOVE));
            Clock;
            set_color(pgm_read_byte(INDEX_MOVE));
            Clock;
            set_color(pgm_read_byte(INDEX_MOVE));
            Clock;
            set_color(pgm_read_byte(INDEX_MOVE));
            Clock;
            set_color(pgm_read_byte(INDEX_MOVE));
            Clock;
            set_color(pgm_read_byte(INDEX_MOVE));
            Clock;
            set_color(pgm_read_byte(INDEX_MOVE));
            Clock;
            set_color(pgm_read_byte(index));
            Clock;
            // shift data into buffers
            HIGH_OE;
            LATCH;
            stepRow();
            CLEAR_OE;
#if MAX_BRIGHTNESS_SLEEP_MUSEC > 0
            delayMicroseconds(BRIGHTNESS_SLEEP_MUSEC / 4);
#if BRIGHTNESS_SLEEP_MUSEC < MAX_BRIGHTNESS_SLEEP_MUSEC

            HIGH_OE;
            delayMicroseconds(MAX_BRIGHTNESS_SLEEP_MUSEC / 4 - BRIGHTNESS_SLEEP_MUSEC / 4);
#endif
#endif
        }
#pragma endregion // LSB

        for (uint8_t y = 0; y < PANEL_Y; y++) // 32 rows
        {
            index = (uint16_t)(buffer + (y << (uint8_t)6)) + (PANEL_BUFFERSIZE * 3 / 4); // advance index to next section

#ifdef PANEL_FLIP_HORIZONTAL
            index += PANEL_X;
#endif
#pragma region LLSB
            set_color(pgm_read_byte(INDEX_MOVE));
            Clock;
            set_color(pgm_read_byte(INDEX_MOVE));
            Clock;
            set_color(pgm_read_byte(INDEX_MOVE));
            Clock;
            set_color(pgm_read_byte(INDEX_MOVE));
            Clock;
            set_color(pgm_read_byte(INDEX_MOVE));
            Clock;
            set_color(pgm_read_byte(INDEX_MOVE));
            Clock;
            set_color(pgm_read_byte(INDEX_MOVE));
            Clock;
            set_color(pgm_read_byte(INDEX_MOVE));
            Clock;

            set_color(pgm_read_byte(INDEX_MOVE));
            Clock;
            set_color(pgm_read_byte(INDEX_MOVE));
            Clock;
            set_color(pgm_read_byte(INDEX_MOVE));
            Clock;
            set_color(pgm_read_byte(INDEX_MOVE));
            Clock;
            set_color(pgm_read_byte(INDEX_MOVE));
            Clock;
            set_color(pgm_read_byte(INDEX_MOVE));
            Clock;
            set_color(pgm_read_byte(INDEX_MOVE));
            Clock;
            set_color(pgm_read_byte(INDEX_MOVE));
            Clock;

            set_color(pgm_read_byte(INDEX_MOVE));
            Clock;
            set_color(pgm_read_byte(INDEX_MOVE));
            Clock;
            set_color(pgm_read_byte(INDEX_MOVE));
            Clock;
            set_color(pgm_read_byte(INDEX_MOVE));
            Clock;
            set_color(pgm_read_byte(INDEX_MOVE));
            Clock;
            set_color(pgm_read_byte(INDEX_MOVE));
            Clock;
            set_color(pgm_read_byte(INDEX_MOVE));
            Clock;
            set_color(pgm_read_byte(INDEX_MOVE));
            Clock;

            set_color(pgm_read_byte(INDEX_MOVE));
            Clock;
            set_color(pgm_read_byte(INDEX_MOVE));
            Clock;
            set_color(pgm_read_byte(INDEX_MOVE));
            Clock;
            set_color(pgm_read_byte(INDEX_MOVE));
            Clock;
            set_color(pgm_read_byte(INDEX_MOVE));
            Clock;
            set_color(pgm_read_byte(INDEX_MOVE));
            Clock;
            set_color(pgm_read_byte(INDEX_MOVE));
            Clock;
            set_color(pgm_read_byte(INDEX_MOVE));
            Clock;

            set_color(pgm_read_byte(INDEX_MOVE));
            Clock;
            set_color(pgm_read_byte(INDEX_MOVE));
            Clock;
            set_color(pgm_read_byte(INDEX_MOVE));
            Clock;
            set_color(pgm_read_byte(INDEX_MOVE));
            Clock;
            set_color(pgm_read_byte(INDEX_MOVE));
            Clock;
            set_color(pgm_read_byte(INDEX_MOVE));
            Clock;
            set_color(pgm_read_byte(INDEX_MOVE));
            Clock;
            set_color(pgm_read_byte(INDEX_MOVE));
            Clock;

            set_color(pgm_read_byte(INDEX_MOVE));
            Clock;
            set_color(pgm_read_byte(INDEX_MOVE));
            Clock;
            set_color(pgm_read_byte(INDEX_MOVE));
            Clock;
            set_color(pgm_read_byte(INDEX_MOVE));
            Clock;
            set_color(pgm_read_byte(INDEX_MOVE));
            Clock;
            set_color(pgm_read_byte(INDEX_MOVE));
            Clock;
            set_color(pgm_read_byte(INDEX_MOVE));
            Clock;
            set_color(pgm_read_byte(INDEX_MOVE));
            Clock;

            set_color(pgm_read_byte(INDEX_MOVE));
            Clock;
            set_color(pgm_read_byte(INDEX_MOVE));
            Clock;
            set_color(pgm_read_byte(INDEX_MOVE));
            Clock;
            set_color(pgm_read_byte(INDEX_MOVE));
            Clock;
            set_color(pgm_read_byte(INDEX_MOVE));
            Clock;
            set_color(pgm_read_byte(INDEX_MOVE));
            Clock;
            set_color(pgm_read_byte(INDEX_MOVE));
            Clock;
            set_color(pgm_read_byte(INDEX_MOVE));
            Clock;

            set_color(pgm_read_byte(INDEX_MOVE));
            Clock;
            set_color(pgm_read_byte(INDEX_MOVE));
            Clock;
            set_color(pgm_read_byte(INDEX_MOVE));
            Clock;
            set_color(pgm_read_byte(INDEX_MOVE));
            Clock;
            set_color(pgm_read_byte(INDEX_MOVE));
            Clock;
            set_color(pgm_read_byte(INDEX_MOVE));
            Clock;
            set_color(pgm_read_byte(INDEX_MOVE));
            Clock;
            set_color(pgm_read_byte(index));
            Clock;
            // shift data into buffers
            HIGH_OE;
            LATCH;
            stepRow();
            CLEAR_OE;
#if MAX_BRIGHTNESS_SLEEP_MUSEC > 0
            delayMicroseconds(BRIGHTNESS_SLEEP_MUSEC / 8);
#if BRIGHTNESS_SLEEP_MUSEC < MAX_BRIGHTNESS_SLEEP_MUSEC

            HIGH_OE;
            delayMicroseconds(MAX_BRIGHTNESS_SLEEP_MUSEC / 8 - BRIGHTNESS_SLEEP_MUSEC / 8);
#endif
#endif
#pragma endregion // LLSB
        }
    }
#endif
#pragma endregion // buffer_specifics
}
;
#endif