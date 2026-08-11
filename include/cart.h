#pragma once
#include "common.h"

#define MAX_CART_ROM_SIZE 0x200000
#define ROM_BANK_SIZE 0x4000
#define RAM_BANK_SIZE 0x2000

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
    u8 rom[MAX_CART_ROM_SIZE];
    u8* ram;
    u8 n_rom_banks, rom_bank;
    u8 n_ram_banks, ram_bank;
    MBC_TYPE_e mbc_type;
    bool ram_enabled, rom_banking_mode;
    char title[16 + 1];
} cart_s;

extern cart_s cart;

void cart_init(void);
void cart_deinit(void);

u8 cart_read(u16 addr);
void cart_write(u16 addr, u8 val);

void mbc_configure(u16 addr, u8 val);
void mbc1_configure(u16 addr, u8 val);