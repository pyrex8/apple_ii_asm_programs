
#include <stdio.h>
#include <stdint.h>
#include "../test_pin/test_pin.h"

#define SPEAKER_ADDR 0xC030
#define SOUND_TOGGLE POKE(SPEAKER_ADDR, 0)


void delay(uint8_t time)
{
    uint8_t i = 0;
    for (i = 0; i < time; i++)
    {
    }
}

void main(void)
{
    uint8_t j = 28;
    printf("SOUND\n");

   while(1)
   {
        TEST_PIN_TOGGLE;
        TEST_PIN_TOGGLE;
        SOUND_TOGGLE;
        SOUND_TOGGLE;
        SOUND_TOGGLE;
        SOUND_TOGGLE;
        SOUND_TOGGLE;
        SOUND_TOGGLE;
        SOUND_TOGGLE;
        delay(j);
        j++;
        if (j > 100)
        {
            j = 28;
        }
   }
}
