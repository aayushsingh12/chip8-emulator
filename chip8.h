#include<iostream>
#include <random>

class Chip8{
public: 
    uint8_t memory[4096]{};
    uint8_t registers[16]{};
    uint16_t index{};
    uint8_t delay{};
    uint8_t sound{};
    uint16_t pc{};
    uint8_t sp{};
    uint16_t stack[16]{};
    uint32_t display[64*32]{}; //0x00000000 is off, 0xFFFFFFFF is on
    uint8_t keypad[16]{}; //0x00 is not pressed, 0xFF is pressed
    uint16_t opcode{};

    Chip8();
    void loadRom(char const * filename);

    void OP_00E0();
    void OP_00EE();
    void OP_1NNN();

private:
    std::default_random_engine randGen; //For random number generation
    std::uniform_int_distribution<uint8_t> randByte;
};
