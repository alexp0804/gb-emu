#include "mem.h"

#include "common.h"

mem_s mem;
u8 cartridge[MAX_CART_SIZE];

bool in_range(u16 addr, u16 lo, u16 hi) { return (lo <= addr && addr <= hi); }

u8 read_mem(u16 addr) {
    if (in_range(addr, ROM_START, ROM_END)) {
        return mem.cart[addr - ROM_START];
    }
    if (in_range(addr, VRAM_START, VRAM_END)) {
        return mem.vram[addr - VRAM_START];
    }
    if (in_range(addr, SRAM_START, SRAM_END)) {
        return mem.sram[addr - SRAM_START];
    }
    if (in_range(addr, WRAM_START, WRAM_END)) {
        return mem.wram[addr - WRAM_START];
    }
    if (in_range(addr, ERAM_START, ERAM_END)) {
        return mem.wram[addr - ERAM_START];
    }
    if (in_range(addr, OAM_START, OAM_END)) {
        return mem.io[addr - OAM_START];
    }
    if (in_range(addr, IO_START, IO_END)) {
        return mem.io[addr - IO_START];
    }
    if (in_range(addr, HRAM_START, HRAM_END)) {
        return mem.hram[addr - HRAM_START];
    }
    if (addr == IE_ADDR) {
        return mem.ie;
    }
    __builtin_unreachable();
}

void write_mem(u16 addr, u8 val) {
    if (in_range(addr, ROM_START, ROM_END)) {
        mem.cart[addr - ROM_START] = val;
    }
    if (in_range(addr, VRAM_START, VRAM_END)) {
        mem.vram[addr - VRAM_START] = val;
    }
    if (in_range(addr, SRAM_START, SRAM_END)) {
        mem.sram[addr - SRAM_START] = val;
    }
    if (in_range(addr, WRAM_START, WRAM_END)) {
        mem.wram[addr - WRAM_START] = val;
    }
    if (in_range(addr, ERAM_START, ERAM_END)) {
        mem.wram[addr - ERAM_START] = val;
    }
    if (in_range(addr, OAM_START, OAM_END)) {
        mem.io[addr - OAM_START] = val;
    }
    if (in_range(addr, IO_START, IO_END)) {
        mem.io[addr - IO_START] = val;
    }
    if (in_range(addr, HRAM_START, HRAM_END)) {
        mem.hram[addr - HRAM_START] = val;
    }
    if (addr == IE_ADDR) {
        mem.ie = val;
    }
}