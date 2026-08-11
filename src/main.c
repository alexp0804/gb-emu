#include <SDL2/SDL.h>
#include <stdio.h>

#include "emu.h"

int main(int argc, char* argv[]) {
    SDL_Event event;
    bool running = true;
    char* rom_file = "Tetris.gb";

    if (argc == 2) {
        rom_file = argv[1];
    }
    if (!read_rom_file(rom_file)) {
        return 1;
    }

    emu_init();
    while (running) {
        emu_step();

        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            }
        }
    }
    emu_deinit();

    return 0;
}
