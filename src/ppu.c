#include "ppu.h"

#include "cpu.h"
#include "mem.h"

ppu_s ppu;
sprite_s oam[OAM_ENTRIES];

enum lcdc_bits {
    PPU_ENABLED = 7,
    WIN_TILEMAP_ADDR = 6,
    WIN_ENABLED = 5,
    BG_WIN_TILEDATA_ADDR = 4,
    BG_TILEMAP_ADDR = 3,
    OBJ_HEIGHT = 2,
    OBJ_ENABLED = 1,
    BG_ENABLED = 0,
};

static bool ppu_enabled(void) {
    return TEST_BIT(ppu.lcd_control, PPU_ENABLED);
}
static bool win_enabled(void) {
    return TEST_BIT(ppu.lcd_control, WIN_ENABLED);
}
static bool obj_enabled(void) {
    return TEST_BIT(ppu.lcd_control, OBJ_ENABLED);
}
static bool bg_enabled(void) {
    return TEST_BIT(ppu.lcd_control, BG_ENABLED);
}
static u16 get_win_tilemap_addr(void) {
    return TEST_BIT(ppu.lcd_control, WIN_TILEMAP_ADDR) ? WIN_TILEMAP_ADDR_HI : WIN_TILEMAP_ADDR_LO;
}
static u16 get_bg_win_tiledata_addr(u8 tile_id) {
    if (TEST_BIT(ppu.lcd_control, BG_WIN_TILEDATA_ADDR))
        return BGWIN_TILEDATA_ADDR_UNSIGNED + (tile_id * BYTES_PER_TILE);
    else
        return BGWIN_TILEDATA_ADDR_SIGNED + ((i8)tile_id * BYTES_PER_TILE);
}
static u16 get_bg_tilemap_addr(void) {
    return TEST_BIT(ppu.lcd_control, BG_TILEMAP_ADDR) ? BG_TILEMAP_ADDR_HI : BG_TILEMAP_ADDR_LO;
}
static u8 get_obj_height(void) {
    return TEST_BIT(ppu.lcd_control, OBJ_HEIGHT) ? 16 : 8;
}
static u8 get_color_id(u8 tiledata_lo, u8 tiledata_hi, u8 bit) {
    return (TEST_BIT(tiledata_hi, bit) << 1) | TEST_BIT(tiledata_lo, bit);
}
static u8 get_palette_color(u8 color_id, u8 palette) {
    return (palette >> (color_id * 2)) & 0b11;
}

static u8 get_bg_win_color(u16 tilemap_addr, u8 bg_x, u8 bg_y) {
    u8 tile_x = bg_x / 8;
    u8 tile_y = bg_y / 8;
    u8 pixel_x = bg_x % 8;
    u8 pixel_y = bg_y % 8;

    u16 tile_index = tile_y * TILES_PER_BG_ROW + tile_x;
    u16 tile_addr = get_bg_win_tiledata_addr(mem_read(tilemap_addr + tile_index));

    u8 data_lo = mem_read(tile_addr + pixel_y * BYTES_PER_TILE_ROW);
    u8 data_hi = mem_read(tile_addr + pixel_y * BYTES_PER_TILE_ROW + 1);
    u8 bit = 7 - pixel_x;

    return get_palette_color(get_color_id(data_lo, data_hi, bit), ppu.bg_palette);
}
static void render_background(void) {
    u16 bg_tilemap_addr = get_bg_tilemap_addr();
    u8 bg_y = ppu.scroll_y + ppu.ly;

    for (int x = 0; x < SCREEN_WIDTH; x++) {
        screen[x][ppu.ly] = get_bg_win_color(bg_tilemap_addr, ppu.scroll_x + x, bg_y);
    }
}
static void render_window(void) {
    if (ppu.window_y > ppu.ly || ppu.window_x > SCREEN_WIDTH)
        return;

    u16 win_tilemap_addr = get_win_tilemap_addr();
    u8 win_x = ppu.window_x - 7;

    for (int x = win_x; x < SCREEN_WIDTH; x++) {
        screen[x][ppu.ly] = get_bg_win_color(win_tilemap_addr, x - win_x, ppu.wly);
    }

    ppu.wly++;
}

static void render_objects(void) {
    u8 sprites_this_scanline = 0;
    u8 sprite_height = get_obj_height();

    // Keeps track of the sprite.x value that drew to the pixel at index i on this scanline.
    // If multiple sprites draw the same pixel the sprite with the smaller sprite.x value is used.
    u8 sprite_priority[SCREEN_WIDTH];
    memset(sprite_priority, UINT8_MAX, sizeof sprite_priority);

    for (u8 i = 0; i < OAM_ENTRIES && sprites_this_scanline < 10; i++) {
        sprite_s sprite = oam[i];
        u8 sprite_y = sprite.y - 16;
        u8 sprite_x = sprite.x - 8;

        // Sprite not on this line
        if (ppu.ly < sprite_y || ppu.ly >= sprite_y + sprite_height)
            continue;

        sprites_this_scanline++;

        u8 pixel_y = ppu.ly - sprite_y;
        if (sprite.flip_y)
            pixel_y = sprite_height - 1 - pixel_y;

        // Get first/second tile for tall sprites
        if (sprite_height == 16) {
            if (pixel_y >= 8) {
                sprite.tile_index |= 1;
                pixel_y -= 8;
            } else {
                sprite.tile_index &= 0xFE;
            }
        }

        u16 tile_addr = VRAM_START + (sprite.tile_index * BYTES_PER_TILE);
        u8 data_lo = mem_read(tile_addr + pixel_y * BYTES_PER_TILE_ROW);
        u8 data_hi = mem_read(tile_addr + pixel_y * BYTES_PER_TILE_ROW + 1);

        for (u8 x = 0; x < 8; x++) {
            if (sprite_x + x >= SCREEN_WIDTH)
                continue;

            u8 pixel_x = sprite.flip_x ? x : (7 - x);
            u8 color_id = get_color_id(data_lo, data_hi, pixel_x);
            u8 color = get_palette_color(color_id,
                                         sprite.dmg_palette ? ppu.obj_palette1 : ppu.obj_palette0);

            if (color_id == 0)
                continue;

            if (sprite.behind_bg && screen[sprite_x + x][ppu.ly] != 0)
                continue;

            if (sprite_priority[sprite_x + x] <= sprite_x)
                continue;

            sprite_priority[sprite_x + x] = sprite_x;
            screen[sprite_x + x][ppu.ly] = color;
        }
    }
}

static void render_scanline(void) {
    if (bg_enabled()) {
        render_background();
        if (win_enabled())
            render_window();
    }
    if (obj_enabled())
        render_objects();
}

static void update_lcd_status(void) {
    ppu.lcd_status &= 0xFC;
    ppu.lcd_status |= ppu.mode;

    WRITE_BIT(ppu.lcd_status, 2, ppu.ly == ppu.lyc);
    if (TEST_BIT(ppu.lcd_status, 2) && TEST_BIT(ppu.lcd_status, 6))
        cpu_request_interrupt(INT_LCD);
}

static void set_mode(PPU_MODE_e mode) {
    if (mode == ppu.mode)
        return;

    ppu.mode = mode;
    update_lcd_status();

    if (mode == MODE_VBLANK)
        cpu_request_interrupt(INT_VBLANK);
    if (mode != MODE_PIXEL_TRANSFER && TEST_BIT((ppu.lcd_status >> 3), mode))
        cpu_request_interrupt(INT_LCD);
}

bool ppu_vram_accessible(void) {
    return ppu.mode != MODE_PIXEL_TRANSFER;
}

bool ppu_oam_accessible(void) {
    return (ppu.mode == MODE_HBLANK || ppu.mode == MODE_VBLANK);
}

void ppu_init(void) {
    ppu = (ppu_s){
        .lcd_control = 0x91,
        .lcd_status = 0x85,
        .bg_palette = 0xFC,
        .wly = 0x00,
    };
}

void ppu_step(u8 cycles) {
    if (!ppu_enabled())
        return;

    ppu.cycle_count += cycles;

    switch (ppu.mode) {
        case MODE_HBLANK:
            if (ppu.cycle_count < HBLANK_DURATION)
                break;
            ppu.cycle_count -= HBLANK_DURATION;
            ppu.ly++;
            set_mode(ppu.ly == 144 ? MODE_VBLANK : MODE_OAM_SCAN);
            break;
        case MODE_VBLANK:
            if (ppu.cycle_count < VBLANK_DURATION)
                break;
            ppu.cycle_count -= VBLANK_DURATION;
            ppu.ly++;
            if (ppu.ly > 153) {
                ppu.ly = ppu.wly = 0;
                set_mode(MODE_OAM_SCAN);
            }
            update_lcd_status();
            break;
        case MODE_OAM_SCAN:
            if (ppu.cycle_count < OAM_SCAN_DURATION)
                break;
            ppu.cycle_count -= OAM_SCAN_DURATION;
            set_mode(MODE_PIXEL_TRANSFER);
            break;
        case MODE_PIXEL_TRANSFER:
            if (ppu.cycle_count < RENDERING_DURATION)
                break;
            ppu.cycle_count -= RENDERING_DURATION;
            update_lcd_status();
            render_scanline();
            set_mode(MODE_HBLANK);
            break;
    }
}