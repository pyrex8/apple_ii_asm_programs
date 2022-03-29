
#include <stdio.h>
#include <stdint.h>
#include "../test_pin/test_pin.h"

#define JOYSTICK_BUTTON0_ADDR 0xC061
#define JOYSTICK_BUTTON1_ADDR 0xC062
#define JOYSTICK_PADDLE0_ADDR 0xC064
#define JOYSTICK_PADDLE1_ADDR 0xC065

#define JOYSTICK_TIMER_RESET_ADDR 0xC070


#define JOYSTICK_FIRE (PEEK(JOYSTICK_BUTTON0_ADDR) > 127 ? 1 : 0)
#define JOYSTICK_START POKE(JOYSTICK_TIMER_RESET_ADDR, 0)
#define JOYSTICK_LEFT (PEEK(JOYSTICK_PADDLE0_ADDR) > 127 ? 0 : 1)
#define JOYSTICK_RIGHT (PEEK(JOYSTICK_PADDLE0_ADDR) > 127 ? 1 : 0)
#define JOYSTICK_UP (PEEK(JOYSTICK_PADDLE1_ADDR) > 127 ? 0 : 1)
#define JOYSTICK_DOWN (PEEK(JOYSTICK_PADDLE1_ADDR) > 127 ? 1 : 0)

void delay(void)
{
    uint8_t i = 0;
    for (i = 0; i < 28; i++)
    {
    }

// too short
// 10 counts = 1040us / 1.5us = 693 clocks
// 15 counts = 1440us / 1.5us = 960 clocks

// right on the edge right doesn't work correctly down does
// 18 counts = 1680us / 1.5us = 1120 clocks

// works
// 19 counts = 1760us / 1.5us = 1173 clocks
// 20 counts = 1880us / 1.5us = 1253 clocks
// 28 counts = 2560us / 1.5us = 1706 clocks
// 30 counts = 2680us / 1.5us = 1786 clocks
// 35 counts = 3160us / 1.5us = 2106 clocks
// 38 counts = 3360us / 1.5us = 2240 clocks

// right on the edge right does work correctly down doesn't
// 39 counts = 3440us / 1.5us = 2293 clocks

// too long
// 40 counts = 3520us / 1.5us = 2347 clocks
}

void main(void)
{
    uint8_t fire = 0;
    uint8_t left = 0;
    uint8_t right = 0;
    uint8_t up = 0;
    uint8_t down = 0;
    printf("JOYSTICK\n");

    while(1)
    {
        fire = JOYSTICK_FIRE;
        JOYSTICK_START;
        left = JOYSTICK_LEFT;
        up = JOYSTICK_UP;
        TEST_PIN_TOGGLE;
        delay();
        TEST_PIN_TOGGLE;
        right = JOYSTICK_RIGHT;
        down = JOYSTICK_DOWN;
        printf("F, L, R, U, D = (%hhu, %hhu, %hhu, %hhu, %hhu)\n", fire, left, right, up, down);
    }
}

// left and up read before 704 clock cycles (11 cycles x 64)
// right and down read after 2112 clock cycles (11 cycles x 64 * 3)

// 11 cycles x 256 = 2816
// 2 to 2673us
// page 1-11, 7-6, 7-33 to 7-36
