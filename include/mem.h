#pragma once
#include "common.h"

#define IO_START 0xFF00
#define IO_END 0xFF7F

typedef struct memory {
} mem_s;

extern mem_s mem;

u8 read_mem(u16 addr);
void write_mem(u16 addr, u8 val);