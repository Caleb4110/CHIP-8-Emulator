#include "window.h"

int main(int argc, char* argv[]) {
    GfxContext ctx;

    gfx_create(&ctx);
    Chip8* chip8 = (Chip8*) new Chip8(LOOP_FREQ, P_CHIP8, time(nullptr));

    SDL_RWops *file = nullptr;
    if (!(file = SDL_RWFromFile(argv[1], "rb"))) {
        SDL_Log("Error: ROM file not found\n");
        return 1;
    }
    uint8_t buff[MAX_ROM_SIZE];
    SDL_RWread(file, buff, 1, MAX_ROM_SIZE);
    SDL_RWclose(file);

    // load program into chip8 memory
    chip8->load_rom(buff, MAX_ROM_SIZE);

    uint64_t start = 0;
    uint64_t end = 0;

    while (true) {
        start = SDL_GetTicks64();
        // Handle input
        if (handle_input(chip8)) break;

        // fetch, decode, execute
        if (chip8->cycle() != 0) {
            SDL_Log("Unknown opcode 0x%x\n", chip8->fetch_opcode());
            gfx_destroy(&ctx);
            return 1;
        }

        // decrement timers
        chip8->decrement_timers();

        // update screen iff screen has been updated
        if (chip8->screen_is_updated()) {
            uint8_t* res = chip8->screen_dump();
            gfx_update(&ctx, res);
        }

        // get time taken to execute everything
        end = SDL_GetTicks64();
        uint32_t total_time = end - start;
        if (total_time < 2)
            SDL_Delay(2-total_time);

        SDL_PumpEvents();
    }

    gfx_destroy(&ctx);
    return 0;
}
