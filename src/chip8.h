#ifndef CHIP8EMULATOR_CHIP8_H
#define CHIP8EMULATOR_CHIP8_H

#include <iostream>
#include <vector>
#include <fstream>
#include <random>
#include <SDL.h>

#define LOOP_FREQ 60

#define RAM_SIZE 0x1000
#define FONT_SIZE 0x50
#define MAX_ROM_SIZE 0xE00 // MEM_SIZE - PC_OFFSET

#define FONT_OFFSET 0x50
#define PC_OFFSET 0x200

#define SCREEN_SCALE_FACTOR 10
#define SCREEN_WIDTH 64
#define SCREEN_HEIGHT 32
#define SCREEN_SIZE (SCREEN_WIDTH * SCREEN_HEIGHT)


#define KEYPAD_SIZE 16

typedef enum {
    P_CHIP8,      // Enable "modern" CHIP-8 behavior
    P_SCHIP_1_0,  // Enable CHIP-48/S-CHIP 1.0 behavior
    P_SCHIP_1_1,  // Enable S-CHIP 1.1 behavior
} Platform;

const uint8_t font[] = {
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

class Chip8 {
    uint8_t RAM[RAM_SIZE];
    uint8_t screen[SCREEN_SIZE];
    uint8_t keypad[KEYPAD_SIZE];


    uint8_t V[16];
    uint16_t stack[16];
    uint8_t SP;

    uint16_t I;
    uint16_t PC;

    uint8_t wait_for_key;

    uint8_t DT;
    uint8_t ST;

    uint16_t opcode;
    uint64_t rng;
    int IPF;

    bool screen_updated;
    Platform platform; // CHIP-8, CHIP-48/S-CHIP 1.0 or S-CHIP 1.1 behavior?

public:
    //void soft_reset(Chip8* vm)
    Chip8(int emu_freq, Platform plt, uint64_t seed);

    void reset();

    int load_rom(unsigned char* rom, int size);

    int cycle();
    uint16_t fetch_opcode();
    int decode_and_execute();

    void decrement_timers();
    bool sound() const;

    bool screen_is_updated() const;
    // bool ended();
    void press_key(int key);
    void release_key(int key);
    //void set_platform(Platform plt);

    // DEBUG FUNCTIONS
    uint8_t* ram_dump();
    uint8_t* screen_dump();
    uint8_t* keypad_dump();
    uint8_t* reg_dump();
    uint16_t* stack_dump();
    uint16_t PC_dump();

    void set_opcode(uint16_t opcode);


    void op_DXYN(uint8_t X, uint8_t Y, uint8_t N);
    void op_FX0A(uint8_t X);
private:





};

#endif //CHIP8EMULATOR_CHIP8_H
