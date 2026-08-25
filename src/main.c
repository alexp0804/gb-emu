#include <SDL2/SDL.h>
#include <stdio.h>

#include "emu.h"
bool debug = false;
bool log_sb = true;

int main(int argc, char* argv[]) {
    SDL_Event event;
    bool running = true;

    if (argc >= 2) {
        if (!read_rom_file(argv[1])) {
            return 1;
        }
    }
    if (argc >= 3) {
        debug = strcmp(argv[2], "--debug") == 0;
        log_sb = !debug;
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
