#pragma once
#include "common.h"
#include "emu.h"

#define LCDC_REG 0xFF40
#define STAT_REG 0xFF41
#define SCY_REG 0xFF42
#define SCX_REG 0xFF43
#define LY_REG 0xFF44
#define LYC_REG 0xFF45
#define DMA_REG 0xFF46
#define BGP_REG 0xFF47
#define OBP0_REG 0xFF48
#define OBP1_REG 0xFF49
#define WY_REG 0xFF4A
#define WX_REG 0xFF4B

#define WIN_TILEMAP_ADDR_LO 0x9800
#define WIN_TILEMAP_ADDR_HI 0x9C00
#define BG_TILEMAP_ADDR_LO 0x9800
#define BG_TILEMAP_ADDR_HI 0x9C00
#define BGWIN_TILEDATA_ADDR_SIGNED 0x9000
#define BGWIN_TILEDATA_ADDR_UNSIGNED 0x8000

#define TILES_PER_BG_ROW 32
#define BYTES_PER_TILE 16
#define BYTES_PER_TILE_ROW 2
#define OAM_ENTRIES 40

#define HBLANK_DURATION 204
#define VBLANK_DURATION 456
#define OAM_SCAN_DURATION 80
#define RENDERING_DURATION 172

typedef enum PPU_MODE {
    MODE_HBLANK = 0,
    MODE_VBLANK,
    MODE_OAM_SCAN,
    MODE_PIXEL_TRANSFER,
} PPU_MODE_e;

typedef enum SPRITE_ATTR_BITS {
    BEHIND_BG = 7,
    FLIP_Y = 6,
    FLIP_X = 5,
    DMG_PALETTE = 4,
    VRAM_BANK = 3,
    // cgb_palette: 2,1,0
} SPRITE_ATTR_BITS_e;

typedef struct ppu {
    u8 lcd_control, lcd_status;
    u8 ly, lyc, wly;
    u8 window_x, window_y;
    u8 scroll_x, scroll_y;
    u8 bg_palette, obj_palette0, obj_palette1;
    PPU_MODE_e mode;
    u32 cycle_count;
} ppu_s;

typedef struct sprite {
    u8 y, x, tile_index;
    bool behind_bg, flip_y, flip_x, dmg_palette;
    bool vram_bank;  // <- These two are unused in DMG
    u8 cgb_palette;  // <-
} sprite_s;

extern ppu_s ppu;
extern sprite_s oam[OAM_ENTRIES];
u8 screen[SCREEN_WIDTH][SCREEN_HEIGHT];

bool ppu_vram_accessible(void);
bool ppu_oam_accessible(void);
void ppu_init(void);
void ppu_step(u8 cycles);