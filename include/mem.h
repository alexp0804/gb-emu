#pragma once
#include "common.h"

#define ROM_START 0x0000
#define ROM_END 0x7FFF
#define VRAM_START 0x8000
#define VRAM_END 0x9FFF
#define SRAM_START 0xA000
#define SRAM_END 0xBFFF
#define WRAM_START 0xC000
#define WRAM_END 0xDFFF
#define ERAM_START 0xE000
#define ERAM_END 0xFDFF
#define OAM_START 0xFE00
#define OAM_END 0xFE9F
#define IO_START 0xFF00
#define IO_END 0xFF7F
#define HRAM_START 0xFF80
#define HRAM_END 0xFFFE
#define IE_ADDR 0xFFFF

#define MAX_CART_SIZE 0x200000

typedef struct memory {
    u8 cart[ROM_END - ROM_START + 1];
    u8 vram[VRAM_END - VRAM_START + 1];
    u8 sram[SRAM_END - SRAM_START + 1];
    u8 wram[WRAM_END - WRAM_START + 1];
    u8 oam[OAM_END - OAM_START + 1];
    u8 io[IO_END - IO_START + 1];
    u8 hram[HRAM_END - HRAM_START + 1];
} mem_s;

extern mem_s mem;
extern u8 cartridge[MAX_CART_SIZE];

u8 read_mem(u16 addr);
void write_mem(u16 addr, u8 val);