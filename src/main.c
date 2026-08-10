#include <stdio.h>

#include "emu.h"

int main(int argc, char* argv[]) {
    char* rom_file = "Tetris.gb";
    if (argc == 2) {
        rom_file = argv[1];
    }
    if (!read_rom_file(rom_file)) {
        return 1;
    }

    emu_init();
    while (true) {
        emu_step();
    }

    return 0;
}
