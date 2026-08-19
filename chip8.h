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

    void OP_00E0(); //Clears screen
    void OP_00EE(); //Return from subroutine
    void OP_1NNN(); //Jump to location NNN
    void OP_2NNN(); //Calls subroutine at NNN
    void OP_3XKK(); //Skip next instruction if Vx=kk
    void OP_4XKK(); //Skip next instruction if Vx!=kk
    void OP_5XY0(); //Skip next instruction if Vx=Vy
    void OP_6XKK(); //Put value kk into register Vx
    void OP_7XKK(); //Adds value kk to value of register Vx, stores result in Vx
    void OP_8XY0(); //Stores value of register Vy in register Vx
    void OP_8XY1(); //Bitwise OR on Vx and Vy, store result in Vx
    void OP_8XY2(); //Bitwise AND on Vx and Vy, store result in Vx
    void OP_8XY3(); //Bitwise XOR on Vx and Vy, store result in Vx
    void OP_8XY4(); //Add Vx and Vy, if > 8 bits, VF set to 1, othersie 0. Lowest 8 bits stored in Vx
    void OP_8XY5(); //If Vx>Vy set VF to 1, otherwise 0. Then subtract Vy from Vx, result in Vx
    void OP_8XY6(); //If LSB of Vx is 1, then set Vf to 1, else 0. Then divide Vx by 2
    void OP_8XY7(); //If Vy>Vx then set Vf to 1, else 0. Then subtract Vx from Vy, result in Vx
    void OP_8XYE(); //If MSB of Vx is 1, then Vf set to 1, else 0. Multiply Vx by 2
    void OP_9XY0(); //Skip next instruction if Vx!=Vy
    void OP_ANNN(); //Value of index register set to NNN
    void OP_BNNN(); //PC value set to NNN plus value of V0
    void OP_CXKK(); //Set Vx=random num from (0, 255) ANDed with kk, stored in Vx
    void OP_DXYN(); //Display n-byte sprite starting at memory location I at (Vx, Vy), Vf = collision
    void OP_EX9E(); //Skip next instruction if key with value of Vx is pressed
    void OP_EXA1(); //Skip next instruction if key with value Vx is NOT pressed
    void OP_FX07(); //Set Vx = delay time value
    void OP_FX0A(); //Store value of key pressed in Vx
    void OP_FX15(); //Set delay timer = Vx
    void OP_FX18(); //Set sound timer = Vx
    void OP_FX1E(); //Add I and Vx and store in I
    void OP_FX29(); //Set I to location of sprite for digit Vx
    void OP_FX33(); //Store BCD of Vx at memory locations I, I+1, I+2
    void OP_FX55(); //Store registers V0 to VX starting at location I
    void OP_FX65(); //Read registers V0 to Vx starting at location I

private:
    std::default_random_engine randGen; //For random number generation
    std::uniform_int_distribution<unsigned int> randByte;
};
