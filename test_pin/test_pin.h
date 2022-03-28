

#ifndef __TEST_PIN_H__
#define __TEST_PIN_H__

#include <peekpoke.h>

#define CASSETTE_OUT_ADDR 0xC020
#define TEST_PIN_ADDR CASSETTE_OUT_ADDR
#define TEST_PIN_TOGGLE POKE(TEST_PIN_ADDR, 0)

#endif /* __TEST_PIN_H__ */
