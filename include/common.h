#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;

#define TEST_BIT(val, bit) ((val >> bit) & 1)
#define SET_BIT(val, bit) (val |= (1 << bit))
#define CLEAR_BIT(val, bit) (val &= ~(1 << bit))
#define WRITE_BIT(val, bit, cond) \
    do {                          \
        if (cond)                 \
            SET_BIT(val, bit);    \
        else                      \
            CLEAR_BIT(val, bit);  \
    } while (0)

#define SCREEN_WIDTH 160
#define SCREEN_HEIGHT 144