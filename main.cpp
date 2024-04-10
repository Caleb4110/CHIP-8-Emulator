#include <iostream>
#include <vector>
#include <fstream>
#include <random>
#include <SDL.h>

#define MEM_SIZE 0x1000
#define FONTSET_SIZE 0x50
#define FONTSET_START_ADDR 0x50
#define PROGRAM_START_ADDR 0x200
#define SCREEN_WIDTH 64
#define SCREEN_HEIGHT 32

SDL_Window* window;
SDL_Renderer* renderer;
int running = 0;

void loadROM(uint8_t memory[MEM_SIZE], char const* filename) {

    std::ifstream file(filename, std::ifstream::binary | std::ios::ate);

    if (file.is_open()) {
        std::streampos size = file.tellg();
        char* buff = (char*) malloc(size * sizeof(char));
        file.seekg(0, std::ifstream::beg);
        file.read(buff, size);
        file.close();

        for(long i = 0; i < size; i++) {
            memory[PROGRAM_START_ADDR + i] = buff[i];
        }

        free(buff);
    }
}

int init_window() {
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        fprintf(stderr, "Error initialising SDL.\n");
        return 0;
    }

    SDL_CreateWindowAndRenderer(SCREEN_WIDTH, SCREEN_HEIGHT, 0, &window, &renderer);

    if (!window) {
        fprintf(stderr, "Error creating SDL window.\n");
        return 0;
    } if (!renderer) {
        fprintf(stderr, "Error creating SDL renderer.\n");
        return 0;
    }
    //SDL_RenderSetScale(renderer, 50, 50)

    return 1;
}

void process_input(uint8_t keypad[16]) {
    SDL_Event event;
    SDL_PollEvent(&event);

    switch (event.type) {
        case SDL_QUIT:
            running = 0;
            break;
        case SDL_KEYDOWN: {
            switch (event.key.keysym.sym) {
                case (SDLK_ESCAPE): {
                    running = 0;
                    break;
                }
                case (SDLK_1): {
                    keypad[0x1] = 1;
                    break;
                }
                case (SDLK_2): {
                    keypad[0x2] = 1;
                    break;
                }
                case (SDLK_3): {
                    keypad[0x3] = 1;
                    break;
                }
                case (SDLK_4): {
                    keypad[0xC] = 1;
                    break;
                }
                case (SDLK_q): {
                    keypad[0x4] = 1;
                    break;
                }
                case (SDLK_w): {
                    keypad[0x5] = 1;
                    break;
                }
                case (SDLK_e): {
                    keypad[6] = 1;
                    break;
                }
                case (SDLK_r): {
                    keypad[0xD] = 1;
                    break;
                }
                case (SDLK_a): {
                    keypad[0x7] = 1;
                    break;
                }
                case (SDLK_s): {
                    keypad[0x8] = 1;
                    break;
                }
                case (SDLK_d): {
                    keypad[0x9] = 1;
                    break;
                }
                case (SDLK_f): {
                    keypad[0xE] = 1;
                    break;
                }
                case (SDLK_z): {
                    keypad[0xA] = 1;
                    break;
                }
                case (SDLK_x): {
                    keypad[0x0] = 1;
                    break;
                }
                case (SDLK_c): {
                    keypad[0xB] = 1;
                    break;
                }
                case (SDLK_v): {
                    keypad[0xF] = 1;
                    break;
                }
            }
            break;
        }
    }
}

void destroy_window() {
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}


int main() {

    uint8_t fontset[FONTSET_SIZE] = {
            0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
            0x20, 0x60, 0x20, 0x20, 0x70, // 1
            0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
            0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
            0x90, 0x90, 0xF0, 0x10, 0x10, // 4
            0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
            0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
            0xF0, 0x10, 0x20, 0x40, 0x40, // 7
            0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
            0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
            0xF0, 0x90, 0xF0, 0x90, 0x90, // A
            0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
            0xF0, 0x80, 0x80, 0x80, 0xF0, // C
            0xE0, 0x90, 0x90, 0x90, 0xE0, // D
            0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
            0xF0, 0x80, 0xF0, 0x80, 0x80  // F
    };
    uint8_t memory[MEM_SIZE] = {0};


    // Add fonts to memory
    for (uint8_t i = 0; i < FONTSET_SIZE; i++) {
        memory[FONTSET_START_ADDR + i] = fontset[i];
    }

    loadROM(memory, "/Users/caleb/Documents/dev/projects/CHIP-8/chip8-roms/games/Tetris [Fran Dachille, 1991].ch8");


    uint8_t PC = 0x0;
    uint16_t indexReg;
    std::vector<uint16_t> stack;
    uint8_t delayTimer;
    uint8_t soundTimer;
    uint8_t keypad[16] = {0};
    uint8_t generalReg[16] = {0};
    uint8_t screen[SCREEN_WIDTH][SCREEN_HEIGHT] = {0};

    running = init_window();

    while(running) {

        uint8_t inst1 = memory[PROGRAM_START_ADDR + PC++];
        uint8_t inst2 = memory[PROGRAM_START_ADDR + PC++];
        uint16_t inst = (inst1 << 8) | inst2;
        printf("%x\n", inst);
        uint8_t T = (inst & 0xF000) >> 12; // instruction type
        uint8_t X = (inst & 0x0F00) >> 8; // second nibble
        uint8_t Y = (inst & 0x00F0) >> 4; // third nibble
        uint8_t N = (inst & 0x000F); // fourth nibble
        uint8_t NN = (inst & 0x00FF); // second byte
        uint16_t NNN = (inst & 0x0FFF); // second third and fourth nibbles
        switch (T) {
            case 0x0: {
                switch (inst) {
                    case 0x00E0: { // clear screen
                        for (int i = 0; i < SCREEN_WIDTH; i++) {
                            for (int j = 0; j < SCREEN_HEIGHT; j++) {
                                screen[i][j] = 0;
                            }
                        }
                        break;
                    } case 0x00EE: { // return from subroutine
                        PC = stack.back();
                        stack.pop_back();
                        break;
                    } default: {
                        break;
                    }
                }
                break;
            } case 0x1: { // jump
                PC = NNN;
                break;
            } case 0x2: { // enter subroutine
                stack.push_back(PC);
                PC = NNN;
                break;
            } case 0x3: { // skip if equal
                if (generalReg[X] == NN)
                    PC += 2;
                break;
            } case 0x4: { // skip if not equal
                if (generalReg[X] != NN)
                    PC += 2;
                break;
            } case 0x5: { // skip if registers equal
                if (generalReg[X] == generalReg[Y])
                    PC += 2;
                break;
            } case 0x6: { // set
                generalReg[X] = NN;
                break;
            } case 0x7: { // add
                generalReg[X] += NN;
                break;
            } case 0x8: {
                switch (N) {
                    case 0x0: // set
                        generalReg[X] = generalReg[Y];
                        break;
                    case 0x1: // binary OR
                        generalReg[X] |= generalReg[Y];
                        break;
                    case 0x2: // binary AND
                        generalReg[X] &= generalReg[Y];
                        break;
                    case 0x3: // logical XOR
                        generalReg[X] ^= generalReg[Y];
                        break;
                    case 0x4: // Add with carry flag
                        if (generalReg[X] + generalReg[Y] > 0xFF)
                            generalReg[0xF] = 1;
                        else
                            generalReg[0xF] = 0;
                        generalReg[X] += generalReg[Y];
                        break;
                    case 0x5: // subtract
                        if (generalReg[X] > generalReg[Y])
                            generalReg[0xF] = 1;
                        else
                            generalReg[0xF] = 0;
                        generalReg[X] -= generalReg[Y];
                        break;
                    case 0x6: // shift to right
                        generalReg[X] = generalReg[Y];
                        if (generalReg[X] & 0x1)
                            generalReg[0xF] = 1;
                        else
                            generalReg[0xF] = 0;
                        generalReg[X] >>= 1;
                        break;
                    case 0x7: // subtract
                        if (generalReg[Y] > generalReg[X])
                            generalReg[0xF] = 1;
                        else
                            generalReg[0xF] = 0;
                        generalReg[Y] -= generalReg[X];
                        break;
                    case 0xE: // shift to left
                        generalReg[X] = generalReg[Y];
                        if (generalReg[X] & 0x80)
                            generalReg[0xF] = 1;
                        else
                            generalReg[0xF] = 0;
                        generalReg[X] <<= 1;
                        break;
                    default:
                        break;
                }
                break;
            } case 0x9: { // skip if equal
                if (generalReg[X] != generalReg[Y]) {
                    PC += 2;
                }
                break;
            } case 0xA: { // set index
                indexReg = NNN;
                break;
            } case 0xB: { // jump with offset
                PC = NNN + generalReg[0x0];
                break;
            } case 0xC: { // random
                uint8_t rand = random();
                generalReg[X] = rand & NN;
                break;
            } case 0xD: { // display
                uint8_t x = generalReg[X] % SCREEN_WIDTH;
                uint8_t y = generalReg[Y] % SCREEN_HEIGHT;
                generalReg[0xF] = 0;
                for (unsigned int row = 0; row < N; row++) {
                    uint8_t spriteRow = memory[indexReg + row];

                    for (unsigned int col = 0; col < 8; col++) {
                        uint8_t pixel = spriteRow & (0x80 >> col);

                        if (pixel) {
                            if(screen[x + col][y + row])
                                generalReg[0xF] = 1;
                            screen[x + col][y + row] ^= 1;
                        }
                        if (x == SCREEN_WIDTH)
                            break;
                    }
                    if (y == SCREEN_HEIGHT)
                        break;
                }
                break;
            } case 0xE: {
                switch (NN) {
                    case 0x9E: { // skip if key
                        if (keypad[generalReg[X]]) {
                            keypad[generalReg[X]] = 0; // TODO: edit
                            PC += 2;
                        }
                        break;
                    } case 0xA1: { // skip if not key
                        if (!keypad[generalReg[X]])
                            PC += 2;
                        break;
                    } default: {
                        break;
                    }
                }
                break;
            } case 0xF: {
                switch (NN) {
                    case 0x07: { // delay timer
                        generalReg[X] = delayTimer;
                        break;
                    } case 0x15: { // set delay timer
                        delayTimer = generalReg[X];
                        break;
                    } case 0x18: { // set sound timer
                        soundTimer = generalReg[X];
                        break;
                    } case 0x1E: { // add to index
                        indexReg == 0x0FFF && generalReg[X] ? generalReg[0xF] = 1: generalReg[0xF] = 0;
                        indexReg += generalReg[X];
                        break;
                    } case 0x0A: { // get key
                        uint8_t pressed = 0;
                        for (int i = 0; i < 16; i++) {
                            if (keypad[i]) {
                                generalReg[X] = i;
                                pressed = 1;
                                keypad[i] = 0;
                            }
                        } if (!pressed)
                            PC -= 2;
                        break;
                    } case 0x29: { // font character
                        indexReg = FONTSET_START_ADDR + (5 * generalReg[X]);
                        break;
                    } case 0x33: { // binary coded decimal conversion
                        memory[indexReg] = (generalReg[X] / 10 / 10);
                        memory[indexReg + 1] = (generalReg[X] / 10) % 10;
                        memory[indexReg + 2] = generalReg[X] % 10;
                        break;
                    } case 0x55: { // store memory
                        for (uint8_t i = 0; i <= X; i++) {
                            memory[indexReg + i] = generalReg[i];
                        }
                        break;
                    } case 0x65: { // load memory
                        for (uint8_t i = 0; i <= X; i++) {
                            generalReg[i] = memory[indexReg + i];
                        }
                        break;
                    } default: {
                        printf("instruction %x not recognised\n", inst);
                    }
                }
                break;
            }
            default: {
                printf("instruction %x not recognised\n", inst);
                break;
            }
        }

        process_input(keypad);

        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        for (int i = 0; i < SCREEN_WIDTH; i++) {
            for (int j = 0; j < SCREEN_HEIGHT; j++) {
                if(screen[i][j]) {
                    SDL_RenderDrawPoint(renderer, i, j);
                }
            }
        }
        SDL_RenderPresent(renderer);
        if (delayTimer)
            delayTimer--;
        if (soundTimer) {
            if (soundTimer == 1)
                printf("BEEP\n");
            soundTimer--;
        }


    }

    destroy_window();

}