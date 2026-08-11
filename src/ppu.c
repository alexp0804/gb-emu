#include "ppu.h"

#include "cpu.h"
#include "mem.h"

ppu_s ppu;

u16 get_win_tilemap_addr(void) {
    return TEST_BIT(ppu.lcd_control, 6) ? WIN_TILEMAP_ADDR_HI : WIN_TILEMAP_ADDR_LO;
}
u16 get_bg_win_tiledata_addr(u8 tile_id) {
    if (TEST_BIT(ppu.lcd_control, 4))
        return BGWIN_TILEDATA_ADDR_UNSIGNED + (tile_id * 16);
    else
        return BGWIN_TILEDATA_ADDR_SIGNED + ((i8)tile_id * 16);
}
u16 get_bg_tilemap_addr(void) {
    return TEST_BIT(ppu.lcd_control, 3) ? BG_TILEMAP_ADDR_HI : BG_TILEMAP_ADDR_LO;
}
u8 get_obj_size(void) { return TEST_BIT(ppu.lcd_control, 2) ? 16 : 8; }

bool obj_is_enabled(void) { return (bool)TEST_BIT(ppu.lcd_control, 1); }
bool win_is_enabled(void) { return (bool)TEST_BIT(ppu.lcd_control, 5); };
bool bg_is_enabled(void) { return (bool)TEST_BIT(ppu.lcd_control, 0); }
bool ppu_is_enabled(void) { return (bool)TEST_BIT(ppu.lcd_control, 7); };

void update_lcd_status() {
    ppu.lcd_status &= 0xFC;
    ppu.lcd_status |= ppu.mode;

    WRITE_BIT(ppu.lcd_status, 2, ppu.ly == ppu.lyc);
    if (TEST_BIT(ppu.lcd_status, 2) && TEST_BIT(ppu.lcd_status, 6)) cpu_request_interrupt(INT_LCD);
}
void set_mode(PPU_MODE_e mode) {
    if (mode == ppu.mode) return;

    ppu.mode = mode;
    update_lcd_status();

    if (mode == MODE_VBLANK) cpu_request_interrupt(INT_VBLANK);
    if (mode != MODE_RENDERING && TEST_BIT((ppu.lcd_status >> 3), mode))
        cpu_request_interrupt(INT_LCD);
}

u8 get_bg_win_color(u16 tilemap_addr, u8 bg_x, u8 bg_y) {
    u8 tile_x = bg_x / 8;
    u8 tile_y = bg_y / 8;
    u8 pixel_x = bg_x % 8;
    u8 pixel_y = bg_y % 8;

    u16 tile_index = tile_y * TILES_PER_BG_ROW + tile_x;
    u16 tile_addr = get_bg_win_tiledata_addr(mem_read(tilemap_addr + tile_index));

    u8 data_lo = mem_read(tile_addr + pixel_y * BYTES_PER_TILE_ROW);
    u8 data_hi = mem_read(tile_addr + pixel_y * BYTES_PER_TILE_ROW + 1);
    u8 bit = 7 - pixel_x;

    return (TEST_BIT(data_hi, bit) << 1) | TEST_BIT(data_lo, bit);
}

void render_background() {
    u16 bg_tilemap_addr = get_bg_tilemap_addr();

    for (int x = 0; x < SCREEN_WIDTH; x++) {
        screen[x][ppu.ly] =
            get_bg_win_color(bg_tilemap_addr, ppu.scroll_x + x, ppu.scroll_y + ppu.ly);
    }
}
void render_window() {
    if (ppu.window_y > ppu.ly) return;
    u16 win_tilemap_addr = get_win_tilemap_addr();
    u8 win_x = ppu.window_x - 7;

    for (int x = win_x; x < SCREEN_WIDTH; x++) {
        screen[x][ppu.ly] = get_bg_win_color(win_tilemap_addr, x - win_x, ppu.ly - ppu.window_y);
    }
}
void render_objects() {}
void render_scanline() {
    if (bg_is_enabled()) {
        render_background();
        if (win_is_enabled()) render_window();
    }
    if (obj_is_enabled()) {
        render_objects();
    }
}

void ppu_init(void) {
    ppu = (ppu_s){
        .lcd_control = 0x91,
        .lcd_status = 0x85,
        .bg_palette = 0xFC,
    };
}
void ppu_step(u8 cycles) {
    if (!ppu_is_enabled()) return;

    ppu.cycle_count += cycles;

    switch (ppu.mode) {
        case MODE_HBLANK:
            if (ppu.cycle_count < HBLANK_DURATION) break;
            ppu.cycle_count -= HBLANK_DURATION;
            ppu.ly++;
            set_mode(ppu.ly == 144 ? MODE_VBLANK : MODE_OAM_SCAN);
            break;
        case MODE_VBLANK:
            if (ppu.cycle_count < VBLANK_DURATION) break;
            ppu.cycle_count -= VBLANK_DURATION;
            ppu.ly++;
            if (ppu.ly > 153) {
                ppu.ly = 0;
                set_mode(MODE_OAM_SCAN);
            }
            update_lcd_status();
            break;
        case MODE_OAM_SCAN:
            if (ppu.cycle_count < OAM_SCAN_DURATION) break;
            ppu.cycle_count -= OAM_SCAN_DURATION;
            set_mode(MODE_RENDERING);
            break;
        case MODE_RENDERING:
            if (ppu.cycle_count < RENDERING_DURATION) break;
            ppu.cycle_count -= RENDERING_DURATION;
            update_lcd_status();
            render_scanline();
            set_mode(MODE_HBLANK);
            break;
    }
}
