
#include <stdio.h>
#include <stdint.h>
#include "../test_pin/test_pin.h"


#include <string.h>
#include <conio.h>
#include <apple2.h>
#include <peekpoke.h>

// peeks, pokes, and strobes
#define STROBE(addr)       __asm__ ("sta %w", addr)

/// HIRES LOOKUP TABLE

#define VHEIGHT 192	// number of scanlines

#define LUT(x) (uint8_t*)(0x2000|x)

// uint8_t xb = DIV7[x];              // divide x by 7
// uint8_t mask = BIT7[MOD7[x]];      // lookup bitmask for remainder
// uint8_t* dest = &vidmem[y][xb];

// starting address of each scanline
static uint8_t* const vidmem[VHEIGHT] = {
  LUT(0x0000), LUT(0x0400), LUT(0x0800), LUT(0x0c00), LUT(0x1000), LUT(0x1400), LUT(0x1800), LUT(0x1c00),
  LUT(0x0080), LUT(0x0480), LUT(0x0880), LUT(0x0c80), LUT(0x1080), LUT(0x1480), LUT(0x1880), LUT(0x1c80),
  LUT(0x0100), LUT(0x0500), LUT(0x0900), LUT(0x0d00), LUT(0x1100), LUT(0x1500), LUT(0x1900), LUT(0x1d00),
  LUT(0x0180), LUT(0x0580), LUT(0x0980), LUT(0x0d80), LUT(0x1180), LUT(0x1580), LUT(0x1980), LUT(0x1d80),
  LUT(0x0200), LUT(0x0600), LUT(0x0a00), LUT(0x0e00), LUT(0x1200), LUT(0x1600), LUT(0x1a00), LUT(0x1e00),
  LUT(0x0280), LUT(0x0680), LUT(0x0a80), LUT(0x0e80), LUT(0x1280), LUT(0x1680), LUT(0x1a80), LUT(0x1e80),
  LUT(0x0300), LUT(0x0700), LUT(0x0b00), LUT(0x0f00), LUT(0x1300), LUT(0x1700), LUT(0x1b00), LUT(0x1f00),
  LUT(0x0380), LUT(0x0780), LUT(0x0b80), LUT(0x0f80), LUT(0x1380), LUT(0x1780), LUT(0x1b80), LUT(0x1f80),
  LUT(0x0028), LUT(0x0428), LUT(0x0828), LUT(0x0c28), LUT(0x1028), LUT(0x1428), LUT(0x1828), LUT(0x1c28),
  LUT(0x00a8), LUT(0x04a8), LUT(0x08a8), LUT(0x0ca8), LUT(0x10a8), LUT(0x14a8), LUT(0x18a8), LUT(0x1ca8),
  LUT(0x0128), LUT(0x0528), LUT(0x0928), LUT(0x0d28), LUT(0x1128), LUT(0x1528), LUT(0x1928), LUT(0x1d28),
  LUT(0x01a8), LUT(0x05a8), LUT(0x09a8), LUT(0x0da8), LUT(0x11a8), LUT(0x15a8), LUT(0x19a8), LUT(0x1da8),
  LUT(0x0228), LUT(0x0628), LUT(0x0a28), LUT(0x0e28), LUT(0x1228), LUT(0x1628), LUT(0x1a28), LUT(0x1e28),
  LUT(0x02a8), LUT(0x06a8), LUT(0x0aa8), LUT(0x0ea8), LUT(0x12a8), LUT(0x16a8), LUT(0x1aa8), LUT(0x1ea8),
  LUT(0x0328), LUT(0x0728), LUT(0x0b28), LUT(0x0f28), LUT(0x1328), LUT(0x1728), LUT(0x1b28), LUT(0x1f28),
  LUT(0x03a8), LUT(0x07a8), LUT(0x0ba8), LUT(0x0fa8), LUT(0x13a8), LUT(0x17a8), LUT(0x1ba8), LUT(0x1fa8),
  LUT(0x0050), LUT(0x0450), LUT(0x0850), LUT(0x0c50), LUT(0x1050), LUT(0x1450), LUT(0x1850), LUT(0x1c50),
  LUT(0x00d0), LUT(0x04d0), LUT(0x08d0), LUT(0x0cd0), LUT(0x10d0), LUT(0x14d0), LUT(0x18d0), LUT(0x1cd0),
  LUT(0x0150), LUT(0x0550), LUT(0x0950), LUT(0x0d50), LUT(0x1150), LUT(0x1550), LUT(0x1950), LUT(0x1d50),
  LUT(0x01d0), LUT(0x05d0), LUT(0x09d0), LUT(0x0dd0), LUT(0x11d0), LUT(0x15d0), LUT(0x19d0), LUT(0x1dd0),
  LUT(0x0250), LUT(0x0650), LUT(0x0a50), LUT(0x0e50), LUT(0x1250), LUT(0x1650), LUT(0x1a50), LUT(0x1e50),
  LUT(0x02d0), LUT(0x06d0), LUT(0x0ad0), LUT(0x0ed0), LUT(0x12d0), LUT(0x16d0), LUT(0x1ad0), LUT(0x1ed0),
  LUT(0x0350), LUT(0x0750), LUT(0x0b50), LUT(0x0f50), LUT(0x1350), LUT(0x1750), LUT(0x1b50), LUT(0x1f50),
  LUT(0x03d0), LUT(0x07d0), LUT(0x0bd0), LUT(0x0fd0), LUT(0x13d0), LUT(0x17d0), LUT(0x1bd0), LUT(0x1fd0)
};

// divide-by 7 table
const uint8_t DIV7[256] = {
 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 4, 4, 4, 4,
 4, 4, 4, 5, 5, 5, 5, 5, 5, 5, 6, 6, 6, 6, 6, 6, 6, 7, 7, 7, 7, 7, 7, 7, 8, 8, 8, 8, 8, 8, 8, 9,
 9, 9, 9, 9, 9, 9,10,10,10,10,10,10,10,11,11,11,11,11,11,11,12,12,12,12,12,12,12,13,13,13,13,13,
13,13,14,14,14,14,14,14,14,15,15,15,15,15,15,15,16,16,16,16,16,16,16,17,17,17,17,17,17,17,18,18,
18,18,18,18,18,19,19,19,19,19,19,19,20,20,20,20,20,20,20,21,21,21,21,21,21,21,22,22,22,22,22,22,
22,23,23,23,23,23,23,23,24,24,24,24,24,24,24,25,25,25,25,25,25,25,26,26,26,26,26,26,26,27,27,27,
27,27,27,27,28,28,28,28,28,28,28,29,29,29,29,29,29,29,30,30,30,30,30,30,30,31,31,31,31,31,31,31,
32,32,32,32,32,32,32,33,33,33,33,33,33,33,34,34,34,34,34,34,34,35,35,35,35,35,35,35,36,36,36,36};

// modulo-by-7 table
const uint8_t MOD7[256] = {
 0, 1, 2, 3, 4, 5, 6, 0, 1, 2, 3, 4, 5, 6, 0, 1, 2, 3, 4, 5, 6, 0, 1, 2, 3, 4, 5, 6, 0, 1, 2, 3,
 4, 5, 6, 0, 1, 2, 3, 4, 5, 6, 0, 1, 2, 3, 4, 5, 6, 0, 1, 2, 3, 4, 5, 6, 0, 1, 2, 3, 4, 5, 6, 0,
 1, 2, 3, 4, 5, 6, 0, 1, 2, 3, 4, 5, 6, 0, 1, 2, 3, 4, 5, 6, 0, 1, 2, 3, 4, 5, 6, 0, 1, 2, 3, 4,
 5, 6, 0, 1, 2, 3, 4, 5, 6, 0, 1, 2, 3, 4, 5, 6, 0, 1, 2, 3, 4, 5, 6, 0, 1, 2, 3, 4, 5, 6, 0, 1,
 2, 3, 4, 5, 6, 0, 1, 2, 3, 4, 5, 6, 0, 1, 2, 3, 4, 5, 6, 0, 1, 2, 3, 4, 5, 6, 0, 1, 2, 3, 4, 5,
 6, 0, 1, 2, 3, 4, 5, 6, 0, 1, 2, 3, 4, 5, 6, 0, 1, 2, 3, 4, 5, 6, 0, 1, 2, 3, 4, 5, 6, 0, 1, 2,
 3, 4, 5, 6, 0, 1, 2, 3, 4, 5, 6, 0, 1, 2, 3, 4, 5, 6, 0, 1, 2, 3, 4, 5, 6, 0, 1, 2, 3, 4, 5, 6,
 0, 1, 2, 3, 4, 5, 6, 0, 1, 2, 3, 4, 5, 6, 0, 1, 2, 3, 4, 5, 6, 0, 1, 2, 3, 4, 5, 6, 0, 1, 2, 3};

// bitmask table
const uint8_t BIT7[7] = { 1, 2, 4, 8, 16, 32, 64 };

/// GRAPHICS FUNCTIONS

// clear screen and set graphics mode
void clrscr() {
  STROBE(0xc052); // turn off mixed-mode
  STROBE(0xc054); // page 1
  STROBE(0xc057); // hi-res
  STROBE(0xc050); // set graphics mode
  memset((uint8_t*)0x2000, 0, 0x2000); // clear page 1
}

// 1ms
void xor_pixel(uint8_t x, uint8_t y)
{
    uint8_t xb = DIV7[x];              // divide x by 7
    uint8_t mask = BIT7[MOD7[x]];      // lookup bitmask for remainder
    uint8_t* dest = &vidmem[y][xb];    // lookup dest. address
    *dest ^= mask;                  // XOR mask with destination
}

// 38us
void pixel()
{
    POKE(0x2000,0xFF);
}

void main(void)
{
    clrscr();
    xor_pixel(100,100);

   while(1)
   {
        TEST_PIN_TOGGLE;
        pixel();
   }
}
