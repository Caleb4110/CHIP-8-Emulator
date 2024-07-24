#include "window.h"

int gfx_create(GfxContext* ctx) {
    if (SDL_Init(SDL_INIT_EVERYTHING))
        return 1;

    ctx->window = SDL_CreateWindow("CHIP-8", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                   SCREEN_WIDTH*SCREEN_SCALE_FACTOR, SCREEN_HEIGHT*SCREEN_SCALE_FACTOR,
                                   SDL_WINDOW_SHOWN);
    if (!ctx->window) {
        gfx_destroy(ctx);
        return 1;
    }

    ctx->renderer = SDL_CreateRenderer(ctx->window, -1, SDL_RENDERER_ACCELERATED);
    if (!ctx->renderer) {
        gfx_destroy(ctx);
        return 1;
    }
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, 0);
    SDL_RenderSetLogicalSize(ctx->renderer, SCREEN_WIDTH, SCREEN_HEIGHT);


    return 0;
}

int gfx_update(GfxContext* ctx, const uint8_t* pixels) {
    SDL_SetRenderDrawColor(ctx->renderer, 0x0, 0x0, 0x0, 0x0);
    SDL_RenderClear(ctx->renderer);
    SDL_SetRenderDrawColor(ctx->renderer, 0xFF, 0xFF, 0xFF, 0xFF);
    for (int y = 0; y < SCREEN_HEIGHT; y++) {
        for (int x = 0; x < SCREEN_WIDTH; x++) {
            if (pixels[y*SCREEN_WIDTH + x]) SDL_RenderDrawPoint(ctx->renderer, x, y);
        }
    }
    SDL_RenderPresent(ctx->renderer);
    return 0;
}

void gfx_destroy(GfxContext* ctx) {
    if (ctx->texture != nullptr) {
        SDL_DestroyTexture(ctx->texture);
        ctx->texture = nullptr;
    }
    if (ctx->renderer != nullptr) {
        SDL_DestroyRenderer(ctx->renderer);
        ctx->renderer = nullptr;
    }
    if (ctx->window != nullptr) {
        SDL_DestroyWindow(ctx->window);
        ctx->window = nullptr;
    }
    SDL_Quit();
}

int handle_input(Chip8* chip8) {
    SDL_Event event;
    bool quit = false;

    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT:
                quit = true;
                break;

            case SDL_KEYDOWN:
                if (event.key.keysym.scancode == SDL_SCANCODE_ESCAPE) {
                    quit = true;
                    break;
                }

                for (int i = 0; i < KEYPAD_SIZE; i++) {
                    if (event.key.keysym.scancode == keys[i])
                        chip8->press_key(i);
                }
                break;

            case SDL_KEYUP:
                for (int i = 0; i < KEYPAD_SIZE; i++) {
                    if (event.key.keysym.scancode == keys[i])
                        chip8->release_key(i);
                }
                break;
        }
    }
    return quit;
}

