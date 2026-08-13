#include "cart.h"

#include "common.h"
#include "mem.h"

cart_s cart;

u32 get_rom_addr(u16 addr) {
    return (cart.rom_bank * ROM_BANK_SIZE) + (addr - ROM_BANK_SIZE);
}
u32 get_ram_addr(u16 addr) {
    return (cart.ram_bank * RAM_BANK_SIZE) + (addr - SRAM_START);
}

void cart_init() {
    strncpy(cart.title, (char*)(cart.rom + 0x134), 16);
    cart.title[16] = '\0';

    // clang-format off
    switch (cart.rom[0x147]) {
        case 0x00:
            cart.mbc_type = MBC_NONE;
            break;
        case 0x01: case 0x02: case 0x03:
            cart.mbc_type = MBC1;
            break;
        case 0x05: case 0x06:
            cart.mbc_type = MBC2;
            break;
        case 0x0F: case 0x10: case 0x11: case 0x12: case 0x13:
            cart.mbc_type = MBC3;
            break;
        case 0x19: case 0x1A: case 0x1B: case 0x1C: case 0x1D: case 0x1E:
            cart.mbc_type = MBC5;
            break;
        case 0x20:
            cart.mbc_type = MBC6;
            break;
        case 0x22:
            cart.mbc_type = MBC7;
            break;
        default:
            cart.mbc_type = MBC_UNKNOWN;
            break;
    }
    // clang-format on

    cart.n_rom_banks = 2 << cart.rom[0x148];

    switch (cart.rom[0x149]) {
        case 0x00:
            cart.n_ram_banks = 0;
            break;
        case 0x02:
            cart.n_ram_banks = 1;
            break;
        case 0x03:
            cart.n_ram_banks = 4;
            break;
        case 0x04:
            cart.n_ram_banks = 16;
            break;
        case 0x05:
            cart.n_ram_banks = 8;
            break;
        default:
            cart.n_ram_banks = 0;
            break;
    }

    if (cart.n_ram_banks != 0) {
        cart.ram = calloc(cart.n_ram_banks * RAM_BANK_SIZE, sizeof(u8));
    }

    cart.rom_bank = 1;
    cart.ram_bank = 0;
    cart.ram_enabled = false;
    cart.rom_banking_mode = true;

    printf("Loaded %s with MBC type %d\n", cart.title, cart.mbc_type);
    printf("ROM size: 0x%x\n", cart.n_rom_banks * ROM_BANK_SIZE);
    printf("# of ROM Banks: %d\n# of RAM Banks: %d\n", cart.n_rom_banks, cart.n_ram_banks);
}
void cart_deinit() {
    if (cart.ram != NULL) {
        free(cart.ram);
    }
}

u8 cart_read(u16 addr) {
    // ROM Bank 0
    if (in_range(addr, ROM_START, ROM_BANK_SIZE - 1)) {
        return cart.rom[addr];
    }
    // ROM Bank 1-N
    if (in_range(addr, ROM_BANK_SIZE, ROM_END)) {
        u32 rom_addr = get_rom_addr(addr);
        if (rom_addr < cart.n_rom_banks * ROM_BANK_SIZE) {
            return cart.rom[rom_addr];
        }
    }

    if (in_range(addr, SRAM_START, SRAM_END)) {
        if (!cart.ram_enabled || cart.n_ram_banks == 0) {
            return 0xFF;
        }
        u32 ram_addr = get_ram_addr(addr);
        if (ram_addr < cart.n_ram_banks * RAM_BANK_SIZE) {
            return cart.ram[ram_addr];
        }
    }
    return 0xFF;
}
void cart_write(u16 addr, u8 val) {
    if (in_range(addr, ROM_START, ROM_END)) {
        mbc_configure(addr, val);
        return;
    }
    if (in_range(addr, SRAM_START, SRAM_END)) {
        if (!cart.ram_enabled || cart.n_ram_banks == 0) {
            return;
        }
        u32 ram_addr = get_ram_addr(addr);
        if (ram_addr < cart.n_ram_banks * RAM_BANK_SIZE) {
            cart.ram[ram_addr] = val;
        }
    }
}

void mbc_configure(u16 addr, u8 val) {
    switch (cart.mbc_type) {
        case MBC_NONE:
            return;
        case MBC1:
            mbc1_configure(addr, val);
            return;
        default:
            printf("Unsupported MBC type!\n");
            exit(1);
    }
}
void mbc1_configure(u16 addr, u8 val) {
    // Configure cart.ram_enabled by writing any value with 0x0A in lower byte.
    if (in_range(addr, 0x0000, 0x1FFF)) {
        cart.ram_enabled = (val & 0x0F) == 0x0A;
        return;
    }
    // Set lower 5bits of cart.rom_bank
    if (in_range(addr, 0x2000, 0x3FFF)) {
        u8 bank_n = val & 0x1F;
        if (bank_n == 0)
            bank_n = 1;
        cart.rom_bank = (cart.rom_bank & 0xE0) | bank_n;
        return;
    }
    // Set the upper 2bits of cart.rom_bank if in ROM banking mode, otherwise set cart.ram_bank.
    if (in_range(addr, 0x4000, 0x5FFF)) {
        u8 val_2bits = val & 0b11;
        if (cart.rom_banking_mode) {
            cart.rom_bank = (val_2bits << 5) | (cart.rom_bank & 0x1F);
        } else {
            cart.ram_bank = val_2bits;
        }
        return;
    }
    // Configure banking mode by writing odd/even number
    if (in_range(addr, 0x6000, 0x7FFF)) {
        cart.rom_banking_mode = !(val & 0b1);
        return;
    }
}