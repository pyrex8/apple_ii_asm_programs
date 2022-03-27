

#ifndef __TEST_PIN_H__
#define __TEST_PIN_H__

#include <peekpoke.h>

#define TEST_PIN_ADDR 0xC0F0
#define TEST_PIN_LOW POKE(TEST_PIN_ADDR, 0)
#define TEST_PIN_HIGH POKE(TEST_PIN_ADDR, 1)

#endif /* __TEST_PIN_H__ */
