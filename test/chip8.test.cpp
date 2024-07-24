#include <gtest/gtest.h>
#include "chip8.h"

class Chip8Test : public testing::Test {
protected:
    Chip8* chip8a = (Chip8*) new Chip8(LOOP_FREQ, P_CHIP8, time(nullptr));

};

TEST_F(Chip8Test, InitZero) {
    uint8_t* screen = chip8a->screen_dump();
    uint8_t* keypad = chip8a->keypad_dump();
    uint8_t* v_regs = chip8a->reg_dump();
    uint16_t* stack = chip8a->stack_dump();

    for (int i = 0; i < SCREEN_SIZE; i++) {
        ASSERT_EQ(screen[i], 0);
    }

    for (int i = 0; i < 16; i++) {
        ASSERT_EQ(keypad[i], 0);
        ASSERT_EQ(v_regs[i], 0);
        ASSERT_EQ(stack[i], 0);
    }
}

// 1NNN: Jump
TEST_F(Chip8Test, JMP) {

    chip8a->set_opcode(0x1232);
    chip8a->decode_and_execute();
    ASSERT_EQ(chip8a->PC_dump(), 0x232);
}

// 2NNN: Enter subroutine,
// 00EE: Return from subroutine
TEST_F(Chip8Test, ENT_and_RET) {
    uint16_t* stack;

    // 2NNN: ENT
    chip8a->set_opcode(0x2400);
    chip8a->decode_and_execute();
    stack = chip8a->stack_dump();
    ASSERT_EQ(chip8a->PC_dump(), 0x400);
    ASSERT_EQ(stack[stack[0]+1], 0x200);

    // 00EE: RET
    chip8a->set_opcode(0x00EE);
    chip8a->decode_and_execute();
    stack = chip8a->stack_dump();
    ASSERT_EQ(chip8a->PC_dump(), 0x200);
    ASSERT_EQ(stack[0], 0);
}

// 3XNN: Skip if VX == NN
TEST_F(Chip8Test, SEQ) {
    // case: equal
    chip8a->set_opcode(0x3000);
    chip8a->decode_and_execute();
    ASSERT_EQ(chip8a->PC_dump(), 0x202);

    // case: not equal
    chip8a->set_opcode(0x3011);
    chip8a->decode_and_execute();
    ASSERT_EQ(chip8a->PC_dump(), 0x202);
}

// 4XNN: Skip if VX != NN
TEST_F(Chip8Test, SNE) {
    // case: equal
    chip8a->set_opcode(0x4000);
    chip8a->decode_and_execute();
    ASSERT_EQ(chip8a->PC_dump(), 0x200);

    // case: not equal
    chip8a->set_opcode(0x4011);
    chip8a->decode_and_execute();
    ASSERT_EQ(chip8a->PC_dump(), 0x202);
}

// 5XY0: Skip if VX == VY
TEST_F(Chip8Test, SREQ) {
    // case: equal
    chip8a->set_opcode(0x5030);
    chip8a->decode_and_execute();
    ASSERT_EQ(chip8a->PC_dump(), 0x202);

    // 6XNN
    // set register for not equal test
    chip8a->set_opcode(0x6321);
    chip8a->decode_and_execute();

    // case: not equal
    chip8a->set_opcode(0x5030);
    chip8a->decode_and_execute();
    ASSERT_EQ(chip8a->PC_dump(), 0x202);
}

// 9XY0: Skip if VX != VY
TEST_F(Chip8Test, SRNE) {
    // case: equal
    chip8a->set_opcode(0x9030);
    chip8a->decode_and_execute();
    ASSERT_EQ(chip8a->PC_dump(), 0x200);

    // 6XNN
    // set register for not equal test
    chip8a->set_opcode(0x6321);
    chip8a->decode_and_execute();

    // case: not equal
    chip8a->set_opcode(0x9030);
    chip8a->decode_and_execute();
    ASSERT_EQ(chip8a->PC_dump(), 0x202);
}

// 6XNN: Set VX to immediate NN
TEST_F(Chip8Test, SETIMM) {
    uint8_t* V;

    chip8a->set_opcode(0x6321);
    chip8a->decode_and_execute();
    chip8a->set_opcode(0x6A10);
    chip8a->decode_and_execute();
    V = chip8a->reg_dump();
    ASSERT_EQ(V[0x3], 0x21);
    ASSERT_EQ(V[0xA], 0x10);
}

// 7XNN: Add NN to VX
TEST_F(Chip8Test, ADD) {
    uint8_t* V;
    chip8a->set_opcode(0x761A);
    chip8a->decode_and_execute();
    V = chip8a->reg_dump();
    ASSERT_EQ(V[0x6], 0x1A);

    chip8a->set_opcode(0x765B);
    chip8a->decode_and_execute();
    V = chip8a->reg_dump();
    ASSERT_EQ(V[0x6], 0x75);
}

// 8XY0: Set VX <- VY
TEST_F(Chip8Test, SETR) {
    // 6XNN: used to set initial VA
    chip8a->set_opcode(0x6A10);
    chip8a->decode_and_execute();

    uint8_t* V = chip8a->reg_dump();
    chip8a->set_opcode(0x8BA0);
    chip8a->decode_and_execute();
    ASSERT_EQ(V[0xA], V[0xB]);
}

// 8XY1: Binary OR
TEST_F(Chip8Test, BOR) {
    // V0 = 0, V0 = 0
    uint8_t* V;
    chip8a->set_opcode(0x8011);
    chip8a->decode_and_execute();
    V = chip8a->reg_dump();
    ASSERT_EQ(V[0], 0x0);

    // 6XNN: set VY
    chip8a->set_opcode(0x6110);
    chip8a->decode_and_execute();

    // V0 = 0, V1 = 0x10
    chip8a->set_opcode(0x8011);
    chip8a->decode_and_execute();
    V = chip8a->reg_dump();
    ASSERT_EQ(V[0], 0x10);
}

// 8XY2: Binary AND
TEST_F(Chip8Test, BAND) {
    // V0 = 0, V0 = 0
    uint8_t* V;
    chip8a->set_opcode(0x8012);
    chip8a->decode_and_execute();
    V = chip8a->reg_dump();
    ASSERT_EQ(V[0], 0x0);

    // 6XNN: set VY
    chip8a->set_opcode(0x6110);
    chip8a->decode_and_execute();

    // V0 = 0, V1 = 0x10
    chip8a->set_opcode(0x8012);
    chip8a->decode_and_execute();
    V = chip8a->reg_dump();
    ASSERT_EQ(V[0], 0x0);

    // 6XNN: set VX
    chip8a->set_opcode(0x6010);
    chip8a->decode_and_execute();

    // V0 = 0x10, V1 = 0x10
    chip8a->set_opcode(0x8012);
    chip8a->decode_and_execute();
    V = chip8a->reg_dump();
    ASSERT_EQ(V[0], 0x10);
}

// 8XY3: Logical XOR
TEST_F(Chip8Test, LXOR) {
    // V0 = 0, V0 = 0
    uint8_t* V;
    chip8a->set_opcode(0x8013);
    chip8a->decode_and_execute();
    V = chip8a->reg_dump();
    ASSERT_EQ(V[0], 0x0);

    // 6XNN: set VY
    chip8a->set_opcode(0x6110);
    chip8a->decode_and_execute();

    // V0 = 0, V1 = 0x10
    chip8a->set_opcode(0x8013);
    chip8a->decode_and_execute();
    V = chip8a->reg_dump();
    ASSERT_EQ(V[0], 0x10);

    // 6XNN: set VX
    chip8a->set_opcode(0x6010);
    chip8a->decode_and_execute();

    // V0 = 0x10, V1 = 0x10
    chip8a->set_opcode(0x8013);
    chip8a->decode_and_execute();
    V = chip8a->reg_dump();
    ASSERT_EQ(V[0], 0x0);
}

// 8XY4: Add with flag
TEST_F(Chip8Test, ADDF) {
    uint8_t* V;

    // 6XNN: set VX
    chip8a->set_opcode(0x6003);
    chip8a->decode_and_execute();

    // 6XNN: set VY
    chip8a->set_opcode(0x6103);
    chip8a->decode_and_execute();

    // Without overflow
    chip8a->set_opcode(0x8014);
    chip8a->decode_and_execute();
    V = chip8a->reg_dump();
    ASSERT_EQ(V[0x0], 0x6);
    ASSERT_EQ(V[0xF], 0);

    // 6XNN: set VX
    chip8a->set_opcode(0x60FF);
    chip8a->decode_and_execute();

    // With overflow
    chip8a->set_opcode(0x8014);
    chip8a->decode_and_execute();
    V = chip8a->reg_dump();
    ASSERT_EQ(V[0x0], 0x2);
    ASSERT_EQ(V[0xF], 1);
}

// 8XY5: Subtract with flag VX - VY
TEST_F(Chip8Test, SUBF) {
    uint8_t* V;

    // 6XNN: set VX
    chip8a->set_opcode(0x6015);
    chip8a->decode_and_execute();

    // 6XNN: set VY
    chip8a->set_opcode(0x6110);
    chip8a->decode_and_execute();

    // 8XY5 Without overflow
    chip8a->set_opcode(0x8015);
    chip8a->decode_and_execute();
    V = chip8a->reg_dump();
    ASSERT_EQ(V[0x0], 0x5);
    ASSERT_EQ(V[0xF], 0x1);

    // 8XY5 With overflow
    chip8a->set_opcode(0x8015);
    chip8a->decode_and_execute();
    V = chip8a->reg_dump();
    ASSERT_EQ(V[0x0], 0xF5);
    ASSERT_EQ(V[0xF], 0x0);

    // 6XNN: set VX
    chip8a->set_opcode(0x6010);
    chip8a->decode_and_execute();

    // 6XNN: set VY
    chip8a->set_opcode(0x6115);
    chip8a->decode_and_execute();

    // 8XY7 With overflow
    chip8a->set_opcode(0x8017);
    chip8a->decode_and_execute();
    V = chip8a->reg_dump();
    ASSERT_EQ(V[0x0], 0x5);
    ASSERT_EQ(V[0xF], 0x1);

    // 6XNN: set VY
    chip8a->set_opcode(0x6104);
    chip8a->decode_and_execute();

    // 8XY7 With overflow
    chip8a->set_opcode(0x8017);
    chip8a->decode_and_execute();
    V = chip8a->reg_dump();
    ASSERT_EQ(V[0x0], 0xFF);
    ASSERT_EQ(V[0xF], 0);
}


int main(int argc, char* argv[]) {
    ::testing::InitGoogleTest(&argc, argv);

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

    return RUN_ALL_TESTS();
}
