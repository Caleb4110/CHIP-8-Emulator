//
// Created by Caleb Walker on 15/7/24.
//

#ifndef CHIP8EMULATOR_WINDOW_H
#define CHIP8EMULATOR_WINDOW_H

#include "SDL.h"
#include "chip8.h"

typedef struct {
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Texture *texture;
} GfxContext;

const char keys[] = {SDL_SCANCODE_X, SDL_SCANCODE_1, SDL_SCANCODE_2, SDL_SCANCODE_3,
                     SDL_SCANCODE_Q, SDL_SCANCODE_W, SDL_SCANCODE_E, SDL_SCANCODE_A,
                     SDL_SCANCODE_S, SDL_SCANCODE_D, SDL_SCANCODE_Z, SDL_SCANCODE_C,
                     SDL_SCANCODE_4, SDL_SCANCODE_R, SDL_SCANCODE_F, SDL_SCANCODE_V};

int gfx_create(GfxContext* ctx);

int gfx_update(GfxContext* ctx, const uint8_t* pixels);

void gfx_destroy(GfxContext* ctx);

int handle_input(Chip8* chip8);

#endif //CHIP8EMULATOR_WINDOW_H
