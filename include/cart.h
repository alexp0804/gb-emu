#pragma once
#include "common.h"

#define ROM_BANK_SIZE 0x4000
#define RAM_BANK_SIZE 0x2000
#define MAX_CART_ROM_SIZE ROM_BANK_SIZE * 128
#define MAX_CART_RAM_SIZE RAM_BANK_SIZE * 16

typedef enum MBC_TYPE {
    MBC_NONE,
    MBC1,
    MBC2,
    MBC3,
    MBC5 = 5,
    MBC6,
    MBC7,
    MBC_UNKNOWN,
} MBC_TYPE_e;

typedef struct cart {
    char title[16 + 1];
    u8 rom[MAX_CART_ROM_SIZE];
    u8 ram[MAX_CART_RAM_SIZE];
    MBC_TYPE_e mbc_type;
    u8 n_rom_banks, n_ram_banks;
    // MBC registers
    u8 bank1, bank2;
    bool mode, ram_enabled;
} cart_s;

extern cart_s cart;

void cart_init(void);
void cart_deinit(void);

u8 cart_rom_read(u16 addr);
void cart_rom_write(u16 addr, u8 val);

u8 cart_ram_read(u16 addr);
void cart_ram_write(u16 addr, u8 val);