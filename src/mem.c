#include "mem.h"

#include "cart.h"
#include "common.h"
#include "cpu.h"
#include "input.h"
#include "ppu.h"
#include "timer.h"

mem_s mem;

bool in_range(u16 addr, u16 lo, u16 hi) {
    return (lo <= addr && addr <= hi);
}

void mem_init() {
    struct initial_values {
        uint16_t addr;
        uint8_t value;
    };

    struct initial_values init_io_values[40] = {
        {0xFF00, 0xCF}, {0xFF01, 0x00}, {0xFF02, 0x7E}, {0xFF04, 0xAB}, {0xFF05, 0x00},
        {0xFF06, 0x00}, {0xFF07, 0xF8}, {0xFF0F, 0xE1}, {0xFF10, 0x80}, {0xFF11, 0xBF},
        {0xFF12, 0xF3}, {0xFF13, 0xFF}, {0xFF14, 0xBF}, {0xFF16, 0x3F}, {0xFF17, 0x00},
        {0xFF18, 0xFF}, {0xFF19, 0xBF}, {0xFF1A, 0x7F}, {0xFF1B, 0xFF}, {0xFF1C, 0x9F},
        {0xFF1D, 0xFF}, {0xFF1E, 0xBF}, {0xFF20, 0xFF}, {0xFF21, 0x00}, {0xFF22, 0x00},
        {0xFF23, 0xBF}, {0xFF24, 0x77}, {0xFF25, 0xF3}, {0xFF26, 0xF1}, {0xFF40, 0x91},
        {0xFF41, 0x85}, {0xFF42, 0x00}, {0xFF43, 0x00}, {0xFF44, 0x00}, {0xFF45, 0x00},
        {0xFF46, 0xFF}, {0xFF47, 0xFC}, {0xFF4A, 0x00}, {0xFF4B, 0x00}, {0xFFFF, 0x00},
    };
    for (int i = 0; i < 40; i++) {
        io_write(init_io_values[i].addr, init_io_values[i].value);
    }

    cart_init();

    mem.cart = cart.rom;
    mem.sram = cart.ram;
}
void mem_deinit() {
    cart_deinit();
}

u8 mem_read(u16 addr) {
    if (in_range(addr, ROM_START, ROM_END)) {
        return cart_rom_read(addr);
    }
    if (in_range(addr, VRAM_START, VRAM_END)) {
        return mem.vram[addr - VRAM_START];
    }
    if (in_range(addr, SRAM_START, SRAM_END)) {
        return cart_ram_read(addr);
    }
    if (in_range(addr, WRAM_START, WRAM_END)) {
        return mem.wram[addr - WRAM_START];
    }
    if (in_range(addr, ERAM_START, ERAM_END)) {
        return mem.wram[addr - ERAM_START];
    }
    if (in_range(addr, OAM_START, OAM_END)) {
        return oam_read(addr);
    }
    if (in_range(addr, IO_START, IO_END)) {
        return io_read(addr);
    }
    if (in_range(addr, HRAM_START, HRAM_END)) {
        return mem.hram[addr - HRAM_START];
    }
    if (addr == IE_REG) {
        return cpu.interrupt_enable;
    }
    __builtin_unreachable();
}
void mem_write(u16 addr, u8 val) {
    if (in_range(addr, ROM_START, ROM_END)) {
        cart_rom_write(addr, val);
    }
    if (ppu_vram_accessible() && in_range(addr, VRAM_START, VRAM_END)) {
        mem.vram[addr - VRAM_START] = val;
    }
    if (in_range(addr, SRAM_START, SRAM_END)) {
        cart_ram_write(addr, val);
    }
    if (in_range(addr, WRAM_START, WRAM_END)) {
        mem.wram[addr - WRAM_START] = val;
    }
    if (in_range(addr, ERAM_START, ERAM_END)) {
        mem.wram[addr - ERAM_START] = val;
    }
    if (ppu_oam_accessible() && in_range(addr, OAM_START, OAM_END)) {
        oam_write(addr, val);
    }
    if (in_range(addr, IO_START, IO_END)) {
        io_write(addr, val);
    }
    if (in_range(addr, HRAM_START, HRAM_END)) {
        mem.hram[addr - HRAM_START] = val;
    }
    if (addr == IE_REG) {
        cpu.interrupt_enable = val;
    }
}

static void dma_transfer(u8 val) {
    u16 src = val << 8;
    for (int i = 0; i < OAM_END - OAM_START + 1; i++) {
        mem_write(OAM_START + i, mem_read(src + i));
    }
}

u8 io_read(u16 addr) {
    switch (addr) {
        case JOYP_REG:
            return input_read();
        case DIV_REG:
        case TIMA_REG:
        case TMA_REG:
        case TAC_REG:
            return timer_read(addr);
        case IF_REG:
            return cpu.interrupt_flag;
        case LCDC_REG:
            return ppu.lcd_control;
        case STAT_REG:
            return ppu.lcd_status;
        case SCY_REG:
            return ppu.scroll_y;
        case SCX_REG:
            return ppu.scroll_x;
        case LY_REG:
            return ppu.ly;
        case LYC_REG:
            return ppu.lyc;
        case BGP_REG:
            return ppu.bg_palette;
        case OBP0_REG:
            return ppu.obj_palette0;
        case OBP1_REG:
            return ppu.obj_palette1;
        case WY_REG:
            return ppu.window_y;
        case WX_REG:
            return ppu.window_x;
        default:
            return mem.io[addr - IO_START];
    }
}
void io_write(u16 addr, u8 val) {
    switch (addr) {
        case JOYP_REG:
            mem.io[JOYP_REG - IO_START] = val & 0x30;
            break;
        case DIV_REG:
        case TIMA_REG:
        case TMA_REG:
        case TAC_REG:
            timer_write(addr, val);
            break;
        case IF_REG:
            cpu.interrupt_flag = val;
            break;
        case LCDC_REG:
            ppu.lcd_control = val;
            break;
        case STAT_REG:
            ppu.lcd_status = val;
            break;
        case SCY_REG:
            ppu.scroll_y = val;
            break;
        case SCX_REG:
            ppu.scroll_x = val;
            break;
        case LY_REG:
            ppu.ly = val;
            break;
        case LYC_REG:
            ppu.lyc = val;
            break;
        case DMA_REG:
            dma_transfer(val);
            break;
        case BGP_REG:
            ppu.bg_palette = val;
            break;
        case OBP0_REG:
            ppu.obj_palette0 = val;
            break;
        case OBP1_REG:
            ppu.obj_palette1 = val;
            break;
        case WY_REG:
            ppu.window_y = val;
            break;
        case WX_REG:
            ppu.window_x = val;
            break;
        default:
            mem.io[addr - IO_START] = val;
            break;
    }
}

u8 oam_read(u16 addr) {
    u16 oam_addr = addr - OAM_START;
    u16 oam_index = oam_addr / 4;
    u16 oam_entry_index = oam_addr % 4;
    sprite_s s = oam[oam_index];

    switch (oam_entry_index) {
        case 0:
            return oam[oam_index].y;
        case 1:
            return oam[oam_index].x;
        case 2:
            return oam[oam_index].tile_index;
        case 3:
            return (s.behind_bg << BEHIND_BG) | (s.flip_y << FLIP_Y) | (s.flip_x << FLIP_X) |
                   (s.dmg_palette << DMG_PALETTE) | (s.vram_bank << VRAM_BANK) |
                   (s.cgb_palette & 0b111);
    }
    __builtin_unreachable();
}
void oam_write(u16 addr, u8 val) {
    u16 oam_addr = addr - OAM_START;
    u16 oam_index = oam_addr / 4;
    u16 oam_entry_index = oam_addr % 4;

    switch (oam_entry_index) {
        case 0:
            oam[oam_index].y = val;
            return;
        case 1:
            oam[oam_index].x = val;
            return;
        case 2:
            oam[oam_index].tile_index = val;
            return;
        case 3:
            oam[oam_index].behind_bg = TEST_BIT(val, BEHIND_BG);
            oam[oam_index].flip_y = TEST_BIT(val, FLIP_Y);
            oam[oam_index].flip_x = TEST_BIT(val, FLIP_X);
            oam[oam_index].dmg_palette = TEST_BIT(val, DMG_PALETTE);
            // These are unused in DMG but setting anyway
            oam[oam_index].vram_bank = TEST_BIT(val, VRAM_BANK);
            oam[oam_index].cgb_palette = val & 0b111;
            return;
    }
}