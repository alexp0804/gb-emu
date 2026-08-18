#include "cart.h"

#include "common.h"
#include "mem.h"

cart_s cart;

// MBC1
static u32 mbc1_rom_addr(u16 addr) {
    u32 bank = 0;
    if (addr <= ROM_BANK_SIZE - 1) {
        bank = (cart.bank2 << 5) * cart.mode;
    } else {
        bank = (cart.bank2 << 5) | (cart.bank1 & 0x1F);
    }
    u32 effective_addr = (bank << 14) | (addr & 0x3FFF);
    // rom chip is addressed by only the bits that fit for that rom size.
    return (cart.n_rom_banks * ROM_BANK_SIZE - 1) & effective_addr;
}
static u8 mbc1_rom_read(u16 addr) {
    return cart.rom[mbc1_rom_addr(addr)];
}
static void mbc1_configure(u16 addr, u8 val) {
    // RAMG (ram enable)
    if (in_range(addr, 0x0000, 0x1FFF)) {
        cart.ram_enabled = (val & 0xF) == 0xA;
    }
    // BANK1
    if (in_range(addr, 0x2000, 0x3FFF)) {
        u8 bank = val & 0x1F;
        if (bank == 0)
            bank = 1;
        cart.bank1 = bank;
    }
    // BANK2
    if (in_range(addr, 0x4000, 0x5FFF)) {
        cart.bank2 = val & 0x3;
    }
    // MODE
    if (in_range(addr, 0x6000, 0x7FFF)) {
        cart.mode = val & 0x1;
    }
}

static u32 mbc1_ram_addr(u16 addr) {
    u32 bank = cart.bank2 * cart.mode;
    u32 effective_addr = (bank << 13) | (addr & 0x1FFF);
    return (cart.n_ram_banks * RAM_BANK_SIZE - 1) & effective_addr;
}
static u8 mbc1_ram_read(u16 addr) {
    if (!cart.ram_enabled || cart.n_ram_banks == 0) {
        return 0xFF;
    }
    return cart.ram[mbc1_ram_addr(addr)];
}
static void mbc1_ram_write(u16 addr, u8 val) {
    if (!cart.ram_enabled || cart.n_ram_banks == 0) {
        return;
    }
    cart.ram[mbc1_ram_addr(addr)] = val;
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

    cart.bank1 = 0b00001;
    cart.bank2 = 0b00;
    cart.mode = 0b0;
    cart.ram_enabled = false;

    printf("Loaded %s with MBC type %d\n", cart.title, cart.mbc_type);
    printf("ROM size: %d bytes\n", cart.n_rom_banks * ROM_BANK_SIZE);
    printf("# of ROM Banks: %d\n# of RAM Banks: %d\n", cart.n_rom_banks, cart.n_ram_banks);
}
void cart_deinit() {
}

static void unsupported_mbc() {
    printf("Unsupported MBC type!\n");
    exit(1);
}

u8 cart_rom_read(u16 addr) {
    switch (cart.mbc_type) {
        case MBC_NONE:
            return cart.rom[addr - ROM_START];
        case MBC1:
            return mbc1_rom_read(addr);
        default:
            unsupported_mbc();
    }
    return 0xFF;
}
void cart_rom_write(u16 addr, u8 val) {
    switch (cart.mbc_type) {
        case MBC_NONE:
            return;
        case MBC1:
            mbc1_configure(addr, val);
            return;
        default:
            unsupported_mbc();
    }
}

u8 cart_ram_read(u16 addr) {
    switch (cart.mbc_type) {
        case MBC_NONE:
            return cart.ram[addr - SRAM_START];
        case MBC1:
            return mbc1_ram_read(addr);
        default:
            unsupported_mbc();
    }
    return 0xFF;
}
void cart_ram_write(u16 addr, u8 val) {
    switch (cart.mbc_type) {
        case MBC_NONE:
            cart.ram[addr - SRAM_START] = val;
            return;
        case MBC1:
            mbc1_ram_write(addr, val);
            return;
        default:
            unsupported_mbc();
    }
}