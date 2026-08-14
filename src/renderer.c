#include "renderer.h"

#include "ppu.h"

video_s video;
u32 palette[4] = {0xFFE0F8D0, 0xFF88C070, 0xFF346856, 0xFF081820};

void renderer_step() {
    // Clear framebuffer, load screen data into framebuffer
    memset(framebuffer, 0, sizeof(framebuffer));

    for (int y = 0; y < SCREEN_HEIGHT; y++) {
        for (int x = 0; x < SCREEN_WIDTH; x++) {
            framebuffer[y * SCREEN_WIDTH + x] = palette[screen[x][y]];
        }
    }

    SDL_UpdateTexture(video.texture, NULL, framebuffer, SCREEN_WIDTH * sizeof(u32));
    SDL_RenderClear(video.renderer);
    SDL_RenderCopy(video.renderer, video.texture, NULL, NULL);
    SDL_RenderPresent(video.renderer);
}
void renderer_init() {
    SDL_Init(SDL_INIT_VIDEO);

    // clang-format off
    video.window = SDL_CreateWindow(
        "Gameboy",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        SCREEN_WIDTH * 5,
        SCREEN_HEIGHT * 5,
        SDL_WINDOW_SHOWN
    );

    video.renderer = SDL_CreateRenderer(
        video.window,
        -1,
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
    );

    video.texture = SDL_CreateTexture(
        video.renderer,
        SDL_PIXELFORMAT_ARGB8888,
        SDL_TEXTUREACCESS_STREAMING,
        SCREEN_WIDTH,
        SCREEN_HEIGHT
    );
    // clang-format on
}
void renderer_deinit() {
    SDL_DestroyTexture(video.texture);
    SDL_DestroyRenderer(video.renderer);
    SDL_DestroyWindow(video.window);
    SDL_Quit();
}