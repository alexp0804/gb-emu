#include <SDL2/SDL.h>

#include "common.h"

typedef struct video {
    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_Texture* texture;
} video_s;

extern video_s video;

u32 framebuffer[SCREEN_WIDTH * SCREEN_HEIGHT];
u32 palette[4];

void renderer_init();
void renderer_step();
void renderer_deinit();