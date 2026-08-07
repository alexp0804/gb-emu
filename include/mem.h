#pragma once
#include "common.h"

typedef struct memory {
} mem_s;

extern mem_s mem;

u8 read_mem(u16 addr);
void write_mem(u16 addr, u8 val);