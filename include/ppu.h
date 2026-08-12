#pragma once
#include "common.h"
#include "emu.h"

#define LCDC_REG 0xFF40
#define STAT_REG 0xFF41
#define SCY_REG 0xFF42
#define SCX_REG 0xFF43
#define LY_REG 0xFF44
#define LYC_REG 0xFF45
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
#define BYTES_PER_TILE_ROW 2

#define HBLANK_DURATION 204
#define VBLANK_DURATION 456
#define OAM_SCAN_DURATION 80
#define RENDERING_DURATION 172

typedef enum PPU_MODE {
    MODE_HBLANK = 0,
    MODE_VBLANK,
    MODE_OAM_SCAN,
    MODE_RENDERING,
} PPU_MODE_e;

typedef struct ppu {
    u8 lcd_control, lcd_status;
    u8 ly, lyc, wly;
    u8 window_x, window_y;
    u8 scroll_x, scroll_y;
    u8 bg_palette, obj_palette0, obj_palette1;
    PPU_MODE_e mode;
    u32 cycle_count;
} ppu_s;

extern ppu_s ppu;
u8 screen[SCREEN_WIDTH][SCREEN_HEIGHT];

u16 get_win_tilemap_addr(void);
u16 get_bg_win_tiledata_addr(u8 tile_id);
u16 get_bg_tilemap_addr(void);
u8 get_obj_size(void);

bool obj_is_enabled(void);
bool win_is_enabled(void);
bool bg_is_enabled(void);
bool ppu_is_enabled(void);

void update_lcd_status();
void set_mode(PPU_MODE_e mode);

void render_background();
void render_window();
void render_objects();
void render_scanline();

void ppu_init(void);
void ppu_step(u8 cycles);