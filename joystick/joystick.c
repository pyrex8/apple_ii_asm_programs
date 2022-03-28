
#include <stdio.h>
#include <stdint.h>
#include "../test_pin/test_pin.h"

#define JOYSTICK_BUTTON0_ADDR 0xC061
#define JOYSTICK_BUTTON1_ADDR 0xC062
#define JOYSTICK_PADDLE0_ADDR 0xC064
#define JOYSTICK_PADDLE1_ADDR 0xC065

#define JOYSTICK_TIMER_RESET_ADDR 0xC070


#define JOYSTICK_FIRE (PEEK(JOYSTICK_BUTTON0_ADDR) > 127 ? 1 : 0)

void main(void)
{
    uint8_t fire = 0;
    printf("JOYSTICK\n");

    while(1)
    {
        TEST_PIN_TOGGLE;
       fire = JOYSTICK_FIRE;
       if (fire)
       {
           printf("fire = %hhu\n", fire);
       }
       TEST_PIN_TOGGLE;
    }
}
