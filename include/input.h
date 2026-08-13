#pragma once
#include "common.h"
#include "mem.h"

#define JOYP_REG 0xFF00

typedef struct input {
    bool start, select, b, a;
    bool up, down, left, right;
} input_s;

extern input_s input;

void input_step(void);
u8 input_read(void);