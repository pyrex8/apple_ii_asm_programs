
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
#define PIXELS 11
#define PIXELS_TOTAL 11

// put pixels in array
// save old location, old mask
// array of x and y, and color
// array of change pixel (limited)
// calcluate new location, new mask
// interate throught old - new pixel (xor pixel function)

static uint8_t pixel_x[PIXELS_TOTAL];
static uint8_t pixel_y[PIXELS_TOTAL];
//static uint8_t pixel_color[PIXELS_TOTAL];
//static uint8_t pixel_new[PIXELS_TOTAL];
static uint8_t pixel_mask_old[PIXELS_TOTAL];
static uint8_t pixel_mask_new[PIXELS_TOTAL];
static uint16_t pixel_addr_old[PIXELS_TOTAL];
static uint16_t pixel_addr_new[PIXELS_TOTAL];

uint8_t pixel;
uint8_t mask;
uint8_t value;
uint16_t addr;


// starting address of each scanline
static const uint16_t vidline[VHEIGHT] =
{
  0x2000, 0x2400, 0x2800, 0x2c00, 0x3000, 0x3400, 0x3800, 0x3c00,
  0x2080, 0x2480, 0x2880, 0x2c80, 0x3080, 0x3480, 0x3880, 0x3c80,
  0x2100, 0x2500, 0x2900, 0x2d00, 0x3100, 0x3500, 0x3900, 0x3d00,
  0x2180, 0x2580, 0x2980, 0x2d80, 0x3180, 0x3580, 0x3980, 0x3d80,
  0x2200, 0x2600, 0x2a00, 0x2e00, 0x3200, 0x3600, 0x3a00, 0x3e00,
  0x2280, 0x2680, 0x2a80, 0x2e80, 0x3280, 0x3680, 0x3a80, 0x3e80,
  0x2300, 0x2700, 0x2b00, 0x2f00, 0x3300, 0x3700, 0x3b00, 0x3f00,
  0x2380, 0x2780, 0x2b80, 0x2f80, 0x3380, 0x3780, 0x3b80, 0x3f80,
  0x2028, 0x2428, 0x2828, 0x2c28, 0x3028, 0x3428, 0x3828, 0x3c28,
  0x20a8, 0x24a8, 0x28a8, 0x2ca8, 0x30a8, 0x34a8, 0x38a8, 0x3ca8,
  0x2128, 0x2528, 0x2928, 0x2d28, 0x3128, 0x3528, 0x3928, 0x3d28,
  0x21a8, 0x25a8, 0x29a8, 0x2da8, 0x31a8, 0x35a8, 0x39a8, 0x3da8,
  0x2228, 0x2628, 0x2a28, 0x2e28, 0x3228, 0x3628, 0x3a28, 0x3e28,
  0x22a8, 0x26a8, 0x2aa8, 0x2ea8, 0x32a8, 0x36a8, 0x3aa8, 0x3ea8,
  0x2328, 0x2728, 0x2b28, 0x2f28, 0x3328, 0x3728, 0x3b28, 0x3f28,
  0x23a8, 0x27a8, 0x2ba8, 0x2fa8, 0x33a8, 0x37a8, 0x3ba8, 0x3fa8,
  0x2050, 0x2450, 0x2850, 0x2c50, 0x3050, 0x3450, 0x3850, 0x3c50,
  0x20d0, 0x24d0, 0x28d0, 0x2cd0, 0x30d0, 0x34d0, 0x38d0, 0x3cd0,
  0x2150, 0x2550, 0x2950, 0x2d50, 0x3150, 0x3550, 0x3950, 0x3d50,
  0x21d0, 0x25d0, 0x29d0, 0x2dd0, 0x31d0, 0x35d0, 0x39d0, 0x3dd0,
  0x2250, 0x2650, 0x2a50, 0x2e50, 0x3250, 0x3650, 0x3a50, 0x3e50,
  0x22d0, 0x26d0, 0x2ad0, 0x2ed0, 0x32d0, 0x36d0, 0x3ad0, 0x3ed0,
  0x2350, 0x2750, 0x2b50, 0x2f50, 0x3350, 0x3750, 0x3b50, 0x3f50,
  0x23d0, 0x27d0, 0x2bd0, 0x2fd0, 0x33d0, 0x37d0, 0x3bd0, 0x3fd0,
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


#define PIXEL_ADDRESS(x, y) (vidline[y] + DIV7[x])
#define PIXEL_MASK(x) (BIT7[MOD7[x]])

/// GRAPHICS FUNCTIONS

// clear screen and set graphics mode
void clrscr() {
  STROBE(0xc052); // turn off mixed-mode
  STROBE(0xc054); // page 1
  STROBE(0xc057); // hi-res
  STROBE(0xc050); // set graphics mode
  memset((uint8_t*)0x2000, 0, 0x2000); // clear page 1
}


void pixel_calc_update(void)
{
    for (pixel = 0; pixel < PIXELS_TOTAL; pixel++)
    {
        pixel_addr_old[pixel] = pixel_addr_new[pixel];
        pixel_addr_new[pixel] = PIXEL_ADDRESS(pixel_x[pixel], pixel_y[pixel]);
        pixel_mask_old[pixel] = pixel_mask_new[pixel];
        pixel_mask_new[pixel] = PIXEL_MASK(pixel_x[pixel]);
    }
}

void pixel_display_update(void)
{
    for (pixel = 0; pixel < PIXELS_TOTAL; pixel++)
    {
        value = PEEK(pixel_addr_old[pixel]);
        value ^= pixel_mask_old[pixel];
        POKE(pixel_addr_old[pixel],value);
        value = PEEK(pixel_addr_new[pixel]);
        value ^= pixel_mask_new[pixel];
        POKE(pixel_addr_new[pixel],value);
    }
}

void main(void)
{
    uint8_t i = 0;
    uint8_t x = 0;
    clrscr();

    while(1)
    {
        TEST_PIN_TOGGLE;
        x += 4;
        for(i = 0; i < PIXELS_TOTAL; i++)
        {
            pixel_x[i] = x;
            pixel_y[i] = 50 + i;
        }
        pixel_calc_update();
        pixel_display_update();
   }
}
