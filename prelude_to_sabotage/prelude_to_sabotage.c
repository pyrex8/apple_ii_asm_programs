// make
// python3 ~/apple_iiv/main.py skydiver.bin

#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <stdint.h>
#include <string.h>
#include <conio.h>
#include <apple2.h>
#include <peekpoke.h>

// type aliases for byte/signed byte/unsigned 16-bit
typedef unsigned char byte;
typedef signed char sbyte;
typedef unsigned short word;

// peeks, pokes, and strobes
#define STROBE(addr)       __asm__ ("sta %w", addr)

#define GAME_ADDR 0xCF00
#define GAME_INDEX_MASK 0xFF
#define GAME_CARD_MEMORY_SIZE (GAME_INDEX_MASK + 1)
#define GAME_SPRITE_MAX 0x40
#define VIDEO_GRAPHICS_MODE 0xC050
#define VIDEO_GRAPHIC_START_ADDR 0x2000

typedef enum
{
    AUDIO_CYCLE_PULSE_COUNT = 0,
    AUDIO_CYCLE_PULSE_FRAMES_1,
    AUDIO_CYCLE_PULSE_FRAMES_2,
    AUDIO_TRANSITION_PULSE_COUNT,
    AUDIO_TRANSITION_PULSE_CLOCKS,
    AUDIO_TRANSITION_PULSE_INCREMENT,
    AUDIO_TRANSITION_PULSE_DECREMENT
} AudioRegisters;

enum
{
    // write only registers
    GAME_SPRITE_POSITION_X  = 0,
    GAME_SPRITE_POSITION_Y  = (GAME_SPRITE_POSITION_X + GAME_SPRITE_MAX),
    GAME_SPRITE_DATA_INDEX  = (GAME_SPRITE_POSITION_Y + GAME_SPRITE_MAX),

    GAME_TILE_X             = (GAME_SPRITE_DATA_INDEX + GAME_SPRITE_MAX),
    GAME_TILE_Y,
    GAME_TILE_INDEX,

    GAME_TOP_TEXT_LINES,
    GAME_BOTTOM_TEXT_LINES,
    GAME_AUDIO_SOUND_INDEX,
    GAME_TEST_PIN,

    GAME_DATA_INDEX,
    GAME_DATA_SET,

    GAME_COLLISION_CLEAR_ALL,
    GAME_COLLISION_NUMBER,

    // read only registers
    GAME_DATA_GET,

    GAME_COLLISION_GET,

    GAME_JOYSTICK_UP,
    GAME_JOYSTICK_DOWN,
    GAME_JOYSTICK_LEFT,
    GAME_JOYSTICK_RIGHT,
    GAME_JOYSTICK_FIRE,
    GAME_RANDOM_NUM,
    GAME_AUDIO_RUNNING,
    GAME_30HZ,
};

enum
{
    CHOPPER_FT = 0,
    CHOPPER_FB,
    CHOPPER_MT,
    CHOPPER_MB,
    CHOPPER_BT,
    CHOPPER_BB,
    PLATFORM_LEFT,
    PLATFORM_RIGHT,
    SKY_DIVER,
    PARACHUTE,
};

#define TEST_PIN_HIGH POKE(GAME_ADDR + GAME_TEST_PIN, 1)
#define TEST_PIN_LOW  POKE(GAME_ADDR + GAME_TEST_PIN, 0)

#define VHEIGHT 192 // number of scanlines
#define VBWIDTH 40  // number of bytes per scanline
#define PIXWIDTH 280    // 7 pixels per byte

const byte sprites[] =
{
    // first sprite blank since all unused sprite reference this one
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    // skydiver
    0x00,
    0x00,
    0x08,
    0x3E,
    0x5D,
    0x1C,
    0x14,
    0x22,
    // parachute
    0x1C,
    0x3E,
    0x7F,
    0x41,
    0x22,
    0x00,
    0x00,
    0x00,
    // grass even
    0x2A,
    0x2A,
    0x2A,
    0x2A,
    0x2A,
    0x2A,
    0x2A,
    0x2A,
    // grass odd
    0x55,
    0x55,
    0x55,
    0x55,
    0x55,
    0x55,
    0x55,
    0x55,
    // platform
    0xFF,
    0xFF,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    // chopper front top
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x7F,
    0x00,
    // chopper front bottom
    0x00,
    0x1F,
    0x30,
    0x7F,
    0x3F,
    0x0F,
    0x42,
    0x3F,
    // chopper middle top
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x60,
    0x20,
    // chopper middle bottom
    0x70,
    0x78,
    0x70,
    0x7F,
    0x7F,
    0x7F,
    0x10,
    0x7C,
    // chopper back top
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    // chopper back bottom
    0x01,
    0x06,
    0x1C,
    0x78,
    0x78,
    0x00,
    0x00,
    0x00,

    // chopper front top
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    // chopper front bottom
    0x00,
    0x1F,
    0x30,
    0x7F,
    0x3F,
    0x0F,
    0x42,
    0x3F,
    // chopper middle top
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x3F,
    0x20,
    // chopper middle bottom
    0x70,
    0x78,
    0x70,
    0x7F,
    0x7F,
    0x7F,
    0x10,
    0x7C,
    // chopper back top
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x70,
    0x00,
    // chopper back bottom
    0x04,
    0x06,
    0x1D,
    0x78,
    0x78,
    0x00,
    0x00,
    0x00,

    // fire sound
    5, // AUDIO_CYCLE_PULSE_COUNT
    3, // AUDIO_CYCLE_PULSE_FRAMES_1
    1, // AUDIO_CYCLE_PULSE_FRAMES_2
    9, // AUDIO_TRANSITION_PULSE_COUNT
    1, // AUDIO_TRANSITION_PULSE_CLOCKS
    1, // AUDIO_TRANSITION_PULSE_INCREMENT
    0, // AUDIO_TRANSITION_PULSE_DECREMENT
    0, // unused

    // chopper sound
    8, // AUDIO_CYCLE_PULSE_COUNT
    3, // AUDIO_CYCLE_PULSE_FRAMES_1
    3, // AUDIO_CYCLE_PULSE_FRAMES_2
    17, // AUDIO_TRANSITION_PULSE_COUNT
    1, // AUDIO_TRANSITION_PULSE_CLOCKS
    0, // AUDIO_TRANSITION_PULSE_INCREMENT
    0, // AUDIO_TRANSITION_PULSE_DECREMENT
    0, // unused

    // explosion sound
    8, // AUDIO_CYCLE_PULSE_COUNT
    3, // AUDIO_CYCLE_PULSE_FRAMES_1
    3, // AUDIO_CYCLE_PULSE_FRAMES_2
    9, // AUDIO_TRANSITION_PULSE_COUNT
    20, // AUDIO_TRANSITION_PULSE_CLOCKS
    0, // AUDIO_TRANSITION_PULSE_INCREMENT
    1, // AUDIO_TRANSITION_PULSE_DECREMENT
    0, // unused

    // experiment sound
    2, // AUDIO_CYCLE_PULSE_COUNT
    1, // AUDIO_CYCLE_PULSE_FRAMES_1
    1, // AUDIO_CYCLE_PULSE_FRAMES_2
    9, // AUDIO_TRANSITION_PULSE_COUNT
    255, // AUDIO_TRANSITION_PULSE_CLOCKS
    0, // AUDIO_TRANSITION_PULSE_INCREMENT
    0, // AUDIO_TRANSITION_PULSE_DECREMENT
    0, // unused
 };


#define SKY_DIVER_DATA 8
#define PARACHUTE_DATA 16
#define GRASS_EVEN_DATA 24
#define GRASS_ODD_DATA 32
#define PLATFORM_DATA 40
#define CHOPPER_FT_DATA 48
#define CHOPPER_FB_DATA 56
#define CHOPPER_MT_DATA 64
#define CHOPPER_MB_DATA 72
#define CHOPPER_BT_DATA 80
#define CHOPPER_BB_DATA 88

#define CHOPPER_FT1_DATA 96
#define CHOPPER_FB1_DATA 104
#define CHOPPER_MT1_DATA 112
#define CHOPPER_MB1_DATA 120
#define CHOPPER_BT1_DATA 128
#define CHOPPER_BB1_DATA 136
#define FIRE_SOUND 144
#define CHOPPER_SOUND 152
#define EXPLOSION_SOUND 160
#define EXPERIMENT_SOUND 168

uint8_t i = 0;
uint8_t j = 0;
uint8_t n = 0;
uint8_t m = 0;

uint8_t p = 0;
uint8_t q = 0;
uint8_t r = 0;
uint8_t s = 0;

uint8_t x = 0;
uint8_t y = 50;

uint8_t v = 0;
uint8_t w = 0;
uint8_t f = 0;
uint8_t up = 0;
uint8_t down = 0;
uint8_t left = 0;
uint8_t right = 0;
uint8_t fire = 0;

uint8_t x_last = 0;
uint8_t y_last = 0;

void graphics_mode(void)
{
    POKE(VIDEO_GRAPHICS_MODE, 0);
}

void text_top(uint8_t line)
{
    POKE(GAME_ADDR + GAME_TOP_TEXT_LINES, line);
}

void text_bottom(uint8_t line)
{
    POKE(GAME_ADDR + GAME_BOTTOM_TEXT_LINES, line);
}

void tile_set(uint8_t loc_x, uint8_t loc_y, uint8_t index)
{
    POKE(GAME_ADDR + GAME_TILE_X, loc_x);
    POKE(GAME_ADDR + GAME_TILE_Y, loc_y);
    POKE(GAME_ADDR + GAME_TILE_INDEX, index);
}

void sprite_data(uint8_t number, uint8_t index)
{
    POKE(GAME_ADDR + GAME_SPRITE_DATA_INDEX + number, index);
}

void sprite_position(uint8_t number, uint8_t x, uint8_t y)
{
    POKE(GAME_ADDR + GAME_SPRITE_POSITION_X + number, x);
    POKE(GAME_ADDR + GAME_SPRITE_POSITION_Y + number, y);
}

uint8_t sprite_collision(uint8_t number)
{
    POKE(GAME_ADDR + GAME_COLLISION_NUMBER, number);
    return PEEK(GAME_ADDR + GAME_COLLISION_GET);
}

void sprite_collision_clear(void)
{
    POKE(GAME_ADDR + GAME_COLLISION_CLEAR_ALL, 1);
}

uint8_t random(void)
{
    return PEEK(GAME_ADDR + GAME_RANDOM_NUM);
}

void sprite_data_copy(const uint8_t *data, uint8_t length)
{
    for (i = 0; i < length; i++)
    {
        POKE(GAME_ADDR + GAME_DATA_INDEX, i);
        POKE(GAME_ADDR + GAME_DATA_SET, data[i]);
    }
}


void chopper_draw(void)
{
    uint8_t frame = r >> 1;

    sprite_data(CHOPPER_FT, CHOPPER_FT_DATA + frame * 48);
    sprite_data(CHOPPER_FB, CHOPPER_FB_DATA + frame * 48);
    sprite_data(CHOPPER_MT, CHOPPER_MT_DATA + frame * 48);
    sprite_data(CHOPPER_MB, CHOPPER_MB_DATA + frame * 48);
    sprite_data(CHOPPER_BT, CHOPPER_BT_DATA + frame * 48);
    sprite_data(CHOPPER_BB, CHOPPER_BB_DATA + frame * 48);

    sprite_position(CHOPPER_FT, p + 14, q);
    sprite_position(CHOPPER_FB, p + 14, q + 8);
    sprite_position(CHOPPER_MT, p + 7, q);
    sprite_position(CHOPPER_MB, p + 7, q + 8);
    sprite_position(CHOPPER_BT, p, q);
    sprite_position(CHOPPER_BB, p, q + 8);
}

void joystick(uint8_t *up, uint8_t *down, uint8_t *left, uint8_t *right, uint8_t *fire)
{
    *up = PEEK(GAME_ADDR + GAME_JOYSTICK_UP);
    *down = PEEK(GAME_ADDR + GAME_JOYSTICK_DOWN);
    *left = PEEK(GAME_ADDR + GAME_JOYSTICK_LEFT);
    *right = PEEK(GAME_ADDR + GAME_JOYSTICK_RIGHT);
    *fire = PEEK(GAME_ADDR + GAME_JOYSTICK_FIRE);
}

void frame_end_wait(void)
{
    while (PEEK(GAME_ADDR + GAME_30HZ) == 0)
    {
    }
    while (PEEK(GAME_ADDR + GAME_30HZ) == 1)
    {
    }
}

void main(void)
{
    graphics_mode();

    text_top(1);
    text_bottom(1);

    sprite_data(SKY_DIVER, SKY_DIVER_DATA);
    sprite_data(PARACHUTE, PARACHUTE_DATA);
    sprite_data(PLATFORM_LEFT, PLATFORM_DATA);
    sprite_data(PLATFORM_RIGHT, PLATFORM_DATA);

    p = 254;
    q = 63;

    sprite_data_copy(sprites, sizeof(sprites));

    for (i = 0; i < 20; i++)
    {
        tile_set(i * 2, 23, GRASS_EVEN_DATA);
        tile_set(i * 2 + 1, 23, GRASS_ODD_DATA);
    }

    x = 254;
    y = 170;

    while (1)
    {
    TEST_PIN_HIGH;

    joystick(&up, &down, &left, &right, &fire);

    x += right - left;
    y += down - up;

    p +=1;

    if(p == 255)
    {
        q = (n>>1) + 20;
        s = (m>>5) * 14 + 26;
        sprite_position(PLATFORM_LEFT, s, 184);
        sprite_position(PLATFORM_RIGHT, s + 7, 184);
    }

    if (x < 1)
    {
        x = 1;
    }
    if (x > 254)
    {
        x = 254;

    }
    if (y > 192)
    {
        y = 192;
    }
    if (y < 1)
    {
        y = 1;
    }

    r = (r + 1) & 0x03;

    if (fire)
    {
        sprite_position(PARACHUTE, x, y - 2);

        f = PEEK(GAME_ADDR + GAME_AUDIO_RUNNING);
        // printf ("\nGAME_AUDIO_RUNNING = %hhu", f);
       if (f == 0)
        {
            POKE(GAME_ADDR + GAME_AUDIO_SOUND_INDEX, EXPLOSION_SOUND);
        }
    }
    else
    {
        sprite_position(PARACHUTE, 10, 30);
    }

    if (sprite_collision(SKY_DIVER))
    {
        sprite_collision_clear();
    }

    n = random();
    m = random();

    sprite_position(SKY_DIVER, x, y);

    chopper_draw();

    // printf is slow, only use for debugging
    // printf ("location %hhu, %hhu \n", x, y);
    // if (x_last != x || y_last != y)
    // {
    //     printf ("\nlocation %hhu, %hhu", x, y);
    // }

    x_last = x;
    y_last = y;

    TEST_PIN_LOW;

    frame_end_wait();
  }
}
