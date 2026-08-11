#pragma once

#include "common.h"

void emu_init(void);
void emu_deinit(void);
void emu_step(void);

bool read_rom_file(char* rom_file);