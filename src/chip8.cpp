#include "chip8.h"

#define DEBUG 0
#define D_PRINT(PC, OP) (printf("PC: %i, OP: 0x%04x\n", PC, OP))


Chip8::Chip8(int emu_freq, Platform plt, uint64_t seed) : RAM(),
V(), stack(), screen(), keypad(), screen_updated() {
    this->I = 0;
    this->SP = 0;
    this->PC = PC_OFFSET;
    this->wait_for_key = 0;
    this->DT = 0;
    this->ST = 0;
    this->opcode = 0;

    this->IPF = static_cast<int>(lround(static_cast<double>(emu_freq) / LOOP_FREQ + 0.5));
    this->platform = plt;
    this->rng = seed;

    for (int i = 0; i < FONT_SIZE; i++)
        this->RAM[FONT_OFFSET + i] = font[i];
}

void Chip8::reset() {
    //memset(this->RAM, 0, RAM_SIZE);
    memset(this->V, 0, 16);
    memset(this->stack, 0, 16);
    memset(this->screen, 0, SCREEN_SIZE);
    memset(this->keypad, 0, 16);

    this->I = 0;
    this->SP = 0;
    this->PC = 0;
    this->wait_for_key = 0;
    this->DT = 0;
    this->ST = 0;
    this->opcode = 0;
}

int Chip8::load_rom(unsigned char* rom, int size) {
    if (size > MAX_ROM_SIZE)
        return 1;
    memcpy(&this->RAM[PC_OFFSET], rom, size);
    return 0;
}

int Chip8::cycle() {
    this->screen_updated = false;
    this->opcode = fetch_opcode();
    return decode_and_execute();
}

void Chip8::decrement_timers() {
    if (this->DT) this->DT--;
    if (this->ST) this->ST--;
}

bool Chip8::sound() const {
    return this->ST > 0;
}

bool Chip8::screen_is_updated() const {
    return this->screen_updated;
}

// bool ended();

void Chip8::press_key(int key) {
    if (key < 0 || key >= 15) std::cout << "KEY OUT OF RANGE" << std::endl;
    this->keypad[key] = 1;
}

void Chip8::release_key(int key) {
    if (key < 0 || key >= 15) std::cout << "KEY OUT OF RANGE" << std::endl;
    this->keypad[key] = 0;
}

//void set_platform(Platform plt)

uint8_t* Chip8::ram_dump() {
    return this->RAM;
}

uint8_t* Chip8::screen_dump() {
    return this->screen;
}

uint8_t* Chip8::keypad_dump() {
    return this->keypad;
}

uint8_t* Chip8::reg_dump() {
    return this->V;
}

uint16_t* Chip8::stack_dump() {
    auto* res = (uint16_t*) calloc(17, sizeof(uint16_t));
    res[0] = this->SP;
    memcpy(&res[1], this->stack, 16);
    return res;
}

void Chip8::set_opcode(uint16_t op) {
    this->opcode = op;
}

uint16_t Chip8::PC_dump() {
    return this->PC;
}

void Chip8::op_DXYN(uint8_t X, uint8_t Y, uint8_t N) {
    uint8_t xc = this->V[X] % SCREEN_WIDTH;
    uint8_t yc = this->V[Y] % SCREEN_HEIGHT;
    this->V[0xF] = 0;

    for (uint32_t row = 0; row < N; row++) {
        uint8_t sprite_pixel = this->RAM[this->I + row];

        for (uint32_t col = 0; col < 8; col++) {
            uint8_t pixel = sprite_pixel & (0x80 >> col);
            if (pixel) {
                if (this->screen[(yc+row)*SCREEN_WIDTH + (xc+col)])
                    this->V[0xF] = 1;
                this->screen[(yc+row)*SCREEN_WIDTH + (xc+col)] ^= 1;
            }
            if (xc + col == SCREEN_WIDTH)
                break;
        }
        if (yc + row == SCREEN_HEIGHT)
            break;
    }
    this->screen_updated = true;
}

void Chip8::op_FX0A(uint8_t X) {
    switch (this->wait_for_key) {
        case 0:
            this->PC -= 2;
            for (unsigned char i : this->keypad)
                if (i) return;
            this->wait_for_key = 1;
            break;

        case 1:
            this->PC -= 2;
            for (int i = 0; i < KEYPAD_SIZE; i++) {
                if (this->keypad[i]) {
                    this->V[X] = i;
                    this->wait_for_key = 2;
                    return;
                }
            }
            break;

        case 2:
            for (int i = 0; i < KEYPAD_SIZE; i++) {
                if (this->keypad[i]) {
                    this->PC -= 2;
                    return;
                }
            }
            this->wait_for_key = 0;
            break;
    }
}

int Chip8::decode_and_execute() {
    uint8_t X = (this->opcode & 0x0F00) >> 8; // second nibble
    uint8_t Y = (this->opcode & 0x00F0) >> 4; // third nibble
    uint8_t N = this->opcode & 0x000F; // fourth nibble
    uint8_t NN = this->opcode & 0x00FF; // second byte
    uint16_t NNN = this->opcode & 0x0FFF; // second third and fourth nibbles
    DEBUG ? D_PRINT(this->PC, this->opcode) : 0;
    switch(this->opcode & 0xF000) {

        case 0x0000:
            if (this->opcode == 0x00E0) { // clear screen
                memset(&this->screen, 0, SCREEN_SIZE);
                this->screen_updated = true;
            } else if (this->opcode == 0x00EE) { // return from subroutine
                if (this->SP <= 0) {
                    std::cout << "ERROR IN RETURN FROM SUBROUTINE" << std::endl;
                    return -1;
                }
                this->PC = this->stack[this->SP--];
            }
            break;

        case 0x1000: // jump
            PC = NNN;
            break;

        case 0x2000: // enter subroutine
            if (this->SP >= 15) {
                std::cout << "ERROR IN ENTER SUBROUTINE" << std::endl;
                return -1;
            }
            this->stack[++this->SP] = this->PC;
            this->PC = NNN;
            break;

        case 0x3000: // skip if equal
            if (this->V[X] == NN) this->PC += 2;
            break;

        case 0x4000: // skip if not equal
            if (this->V[X] != NN) this->PC += 2;
            break;

        case 0x5000: // skip if equal
            if (this->V[X] == this->V[Y]) this->PC += 2;
            break;

        case 0x6000: // set
            this->V[X] = NN;
            break;

        case 0x7000: // add
            this->V[X] += NN;
            break;

        case 0x8000:
            switch (this->opcode & 0x000F) {
                case 0x0000: // set
                    this->V[X] = this->V[Y];
                    break;

                case 0x0001: // binary OR
                    this->V[X] |= this->V[Y];
                    break;

                case 0x0002: // binary AND
                    this->V[X] &= this->V[Y];
                    break;

                case 0x0003: // logical XOR
                    this->V[X] ^= this->V[Y];
                    break;

                case 0x0004: { // Add with carry flag
                    bool carry = (this->V[X] + this->V[Y]) > 0xFF;
                    this->V[X] += this->V[Y];
                    this->V[0xF] = carry;
                    break;
                }

                case 0x0005: { // subtract
                    bool flag = this->V[X] > this->V[Y];
                    this->V[X] -= this->V[Y];
                    this->V[0xF] = flag;
                    break;
                }

                case 0x0006: { // shift to right
                    this->V[X] = this->V[Y]; // only for platform CHIP-8
                    uint8_t flag = this->V[X] & 0x01;
                    this->V[X] >>= 1;
                    this->V[0xF] = flag;
                    break;
                }

                case 0x0007: { // subtract
                    bool flag = this->V[Y] > this->V[X];
                    this->V[X] = this->V[Y] - this->V[X];
                    this->V[0xF] = flag;
                    break;
                }

                case 0x000E: { // shift to left
                    this->V[X] = this->V[Y]; // only for platform CHIP-8
                    bool flag = this->V[X] >> 7;
                    V[X] <<= 1;
                    this->V[0xF] = flag;
                    break;
                }

                default:
                    return -1;
            }
            break;

        case 0x9000: // skip not equal
            if (this->V[X] != this->V[Y]) PC += 2;
            break;

        case 0xA000: // set index
            this->I = NNN;
            break;

        case 0xB000: // jump with offset
            this->PC = NNN + this->V[0x0];
            break;

        case 0xC000: { // random
            uint8_t rand = random();
            this->V[X] = rand & NN;
            break;
        }

        case 0xD000:
            op_DXYN(X, Y, N);
            break;

        case 0xE000:
            switch (this->opcode & 0x00FF) {
                case 0x009E: // skip if key
                    if (this->keypad[this->V[X]]) this->PC += 2;
                    break;

                case 0x00A1: // skip if not key
                    if (!this->keypad[this->V[X]]) this->PC += 2;
                    break;

                default:
                    return -1;
            }
            break;

        case 0xF000:
            switch (this->opcode & 0x00FF) {
                case 0x0007: // VX set to delay timer
                    this->V[X] = this->DT;
                    break;

                case 0x0015: // delay timer set to VX
                    this->DT = this->V[X];
                    break;

                case 0x0018: // sound timer set to VX
                    this->ST = this->V[X];
                    break;

                case 0x001E: // add VX to I
                    this->I += this->V[X];
                    break;

                case 0x000A: // wait for key input
                    op_FX0A(X);
                    break;

                case 0x0029: // set I to hex character in V[X]
                    this->I = FONT_OFFSET + (this->V[X] * 5); // font sprite is 5 bytes
                    break;

                case 0x0033: // binary coded decimal conversion
                    this->RAM[this->I] = (this->V[X] / 100) % 10;
                    this->RAM[this->I + 1] = (this->V[X] / 10) & 10;
                    this->RAM[this->I + 2] = this->V[X] % 10;
                    break;

                case 0x0055:
                    for (int i = 0; i <= X; i++)
                        this->RAM[this->I + i] = this->V[i];

                    this->I += (X + 1); // // only for platform CHIP-8
                    break;

                case 0x0065:
                    for (int i = 0; i <= X; i++)
                        this->V[i] = this->RAM[this->I + i];

                    this->I += (X + 1); // // only for platform CHIP-8
                    break;

                default:
                    return -1;
            }
            break;

        default:
            return -1;
    }
    DEBUG ? printf("\n") : 0;
    return 0;
}

uint16_t Chip8::fetch_opcode() {
    uint8_t first = this->RAM[this->PC++];
    uint8_t second = this->RAM[this->PC++];
    this->opcode = (first << 8) | second;
    return this->opcode;
}