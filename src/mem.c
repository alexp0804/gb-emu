#include "mem.h"

#include "common.h"

mem_s mem;
u8 cartridge[MAX_CART_SIZE];

u8 read_mem(u16 addr) { return 0; }
void write_mem(u16 addr, u8 val) { return; }