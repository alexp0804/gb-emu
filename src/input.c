#include "input.h"

#include <SDL2/SDL.h>

#include "mem.h"

input_s input;

void input_step(void) {
    const u8* keys = SDL_GetKeyboardState(NULL);

    input.a = keys[SDL_SCANCODE_J];
    input.b = keys[SDL_SCANCODE_K];
    input.start = keys[SDL_SCANCODE_RETURN];
    input.select = keys[SDL_SCANCODE_BACKSPACE];
    input.up = keys[SDL_SCANCODE_W];
    input.left = keys[SDL_SCANCODE_A];
    input.down = keys[SDL_SCANCODE_S];
    input.right = keys[SDL_SCANCODE_D];
}

u8 input_read(void) {
    u8 joyp = mem.io[JOYP_REG - IO_START] | 0xcf;
    if (!TEST_BIT(joyp, 4)) {
        WRITE_BIT(joyp, 0, !input.right);
        WRITE_BIT(joyp, 1, !input.left);
        WRITE_BIT(joyp, 2, !input.up);
        WRITE_BIT(joyp, 3, !input.down);
    }
    if (!TEST_BIT(joyp, 5)) {
        WRITE_BIT(joyp, 0, !input.a);
        WRITE_BIT(joyp, 1, !input.b);
        WRITE_BIT(joyp, 2, !input.select);
        WRITE_BIT(joyp, 3, !input.start);
    }
    return joyp;
}