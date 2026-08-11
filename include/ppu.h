#pragma once
#include "common.h"
#include "emu.h"

typedef enum PPU_MODE {
    MODE_HBLANK,
    MODE_VBLANK,
    MODE_OAM_SCAN,
    MODE_RENDERING,
} PPU_MODE_e;

u8 screen[SCREEN_WIDTH][SCREEN_HEIGHT];