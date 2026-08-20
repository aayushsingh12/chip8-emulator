#include <iostream>
#include <fstream>
#include <chrono>
#include "chip8.h"

const unsigned int START_ADDRESS = 0x200;   
const unsigned int FONT_ADDRESS = 0X50;
const unsigned int FONT_SIZE= 80;   //Avoid magic numbers!
const unsigned int DISPLAY_WIDHT = 64;
const unsigned int DISPLAY_HEIGHT = 32;

const uint8_t font[FONT_SIZE]={
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

Chip8::Chip8()
    :randGen(std::chrono::system_clock::now().time_since_epoch().count())
{
    pc=START_ADDRESS;

    for(int i=0; i<FONT_SIZE; i++){
        memory[FONT_ADDRESS+i]=font[i];
    }

    randByte = std::uniform_int_distribution<unsigned int>(0, 255U); 

    /*
    Essentially, whenever a chip8 object is created, the random number 
    generator is seeded wrt the current time, and then a u_int8 is
    configured to be generated. This is dont to generate random
    numbers. The random number has not been generated yet.
    */
}

void Chip8::loadRom(char const * filename){
    std::ifstream file(filename, std::ios::binary | std::ios::ate); //ifstream is just to read
    //current pointer is at end(due to std::ios::ate)
    if(!file.is_open()){
        std::cerr<<"Failed to open ROM:"<<filename<<std::endl;
        return;
    }
    auto size= file.tellg();    //returns current read pos, which is at end
    char * buffer = new char[size]; //dynamic allocation of a char array of size size
    file.seekg(0, std::ios::beg);   //rewind to start(0 from beginning)
    file.read(buffer, size);
    file.close();
    for(long i=0; i<size; i++){
        memory[START_ADDRESS+i]=buffer[i];
    }
    
    delete[] buffer;
}

void Chip8::OP_00E0(){
    //Clears screen
    for(unsigned int i=0; i<DISPLAY_WIDHT* DISPLAY_HEIGHT; i++)
        display[i]=0x00000000;
}

void Chip8::OP_00EE(){
    //Return from subroutine
    if(sp==0){
        std::cout<<"Invalid instruction; stack pointer at bottom"<<std::endl;
        return;
    }
    pc= stack[sp];
    sp--;
}

void Chip8::OP_1NNN(){
    //Jumps to location NNN
    uint16_t address= opcode & 0x0FFFu;
    pc= address;
}

void Chip8::OP_2NNN(){
    //Calls subroutine at NNN
    uint16_t address= opcode & 0x0FFFu;
    stack[++sp]=pc; //increments stack pointer first
    pc=address;
}

void Chip8::OP_3XKK(){
    //Skip next instruction if Vx=kk
    uint8_t Vx= (opcode & 0x0F00u) >> 8;    //Shift right by 8 bits
    uint8_t kk= (opcode & 0x00FFu);
    if(registers[Vx]==kk)
        pc+=2;
}

void Chip8::OP_4XKK(){
    //Skip next instruction if Vx!=kk
    uint8_t Vx= (opcode & 0x0F00u) >> 8;
    uint8_t kk= (opcode & 0x00FF);
    if(registers[Vx]!=kk)
        pc+=2;
}

void Chip8::OP_5XY0(){
    //Skip next instruction if Vx=Vy
    uint8_t Vx= (opcode & 0x0F00u) >> 8;
    uint8_t Vy= (opcode & 0x00F0u) >> 4;
    if(registers[Vx]==registers[Vy])
        pc+=2;
}

void Chip8::OP_6XKK(){
    //Put value kk into register Vx
    uint8_t kk= opcode & 0x00FFu;
    uint8_t Vx= (opcode & 0x0F00) >> 8;
    registers[Vx]=kk;
}

void Chip8::OP_7XKK(){
    ////Adds value kk to value of register Vx, stores result in Vx
    uint8_t kk= opcode & 0x00FFu;
    uint8_t Vx=  (opcode & 0x0F00u) >> 8;
    registers[Vx]+=kk;
}

void Chip8::OP_8XY0(){
    //Stores value of register Vy in register Vx
    uint8_t Vx= (opcode & 0x0F00) >> 8;
    uint8_t Vy= (opcode & 0x00F0) >> 4;
    registers[Vx]=registers[Vy];
}

void Chip8::OP_8XY1(){
    //Bitwise OR on Vx and Vy, store result in Vx
    uint8_t Vx= (opcode & 0x0F00) >> 8;
    uint8_t Vy= (opcode & 0x00F0) >> 4;
    registers[Vx]= registers[Vx] | registers[Vy];
}

void Chip8::OP_8XY2(){
    //Bitwise AND on Vx and Vy, store result in Vx
    uint8_t Vx= (opcode & 0x0F00) >> 8;
    uint8_t Vy= (opcode & 0x00F0) >> 4;
    registers[Vx]= registers[Vx] & registers[Vy];
}

void Chip8::OP_8XY3(){
    //Bitwise XOR on Vx and Vy, store result in Vx
    uint8_t Vx= (opcode & 0x0F00) >> 8;
    uint8_t Vy= (opcode & 0x00F0) >> 4;
    registers[Vx]= registers[Vx] ^ registers[Vy];
}

void Chip8::OP_8XY4(){
    //Add Vx and Vy, if > 8 bits, VF set to 1, othersie 0. Lowest 8 bits stored in Vx
    uint8_t Vx= (opcode & 0x0F00u) >> 8;
    uint8_t Vy= (opcode & 0x00F0u) >> 4;
    uint16_t sum= registers[Vx] + registers[Vy];

    if(sum > 0xFFu)
        registers[0xFu]= 1;
    else    
        registers[0xFu]=0;
    
    registers[Vx] = sum & 0x00FFu;
}

void Chip8::OP_8XY5(){
    //If Vx>Vy set VF to 1, otherwise 0. Then subtract Vy from Vx, result in Vx
    uint8_t Vx= (opcode & 0x0F00u) >> 8;
    uint8_t Vy= (opcode & 0x00F0u) >> 4;

    if(registers[Vx]>registers[Vy])
        registers[0xFu]= 1;
    else    
        registers[0xFu]= 0;

    registers[Vx] -= registers[Vy];
}

void Chip8::OP_8XY6(){
    //If LSB of Vx is 1, then set Vf to 1, else 0. Then divide Vx by 2
    uint8_t Vx= (opcode & 0x0F00u) >> 8;
    uint8_t Vy= (opcode & 0x00F0u) >> 4;

    if((registers[Vx] & 0x01u) == 1)
        registers[0xFu]= 1;
    else    
        registers[0xFu]= 0;
    
    registers[Vx] /= 2;
}

void Chip8::OP_8XY7(){
    //If Vy>Vx then set Vf to 1, else 0. Then subtract Vx from Vy, result in Vx
    uint8_t Vx= (opcode & 0x0F00u) >> 8;
    uint8_t Vy= (opcode & 0x00F0u) >> 4;

    if(registers[Vy]>registers[Vx])
        registers[0xFu]= 1;
    else    
        registers[0xFu]= 0;

    registers[Vx] = registers[Vy] - registers[Vx];
}

void Chip8::OP_8XYE(){
    //If MSB of Vx is 1, then Vf set to 1, else 0. Multiply Vx by 2
    uint8_t Vx= (opcode & 0x0F00u) >> 8;
    uint8_t Vy= (opcode & 0x00F0u) >> 4;

    if(((registers[Vx] & 0x80u) >> 7) == 1)
        registers[0xFu]=1;
    else    
        registers[0xFu]=0;
    
    registers[Vx] *= 2;
}

void Chip8::OP_9XY0(){
    //Skip next instruction if Vx!=Vy
    uint8_t Vx= (opcode & 0x0F00u) >> 8;
    uint8_t Vy= (opcode & 0x00F0u) >> 4;

    if(registers[Vx]!=registers[Vy])
        pc+=2;
}

void Chip8::OP_ANNN(){
    //Value of index register set to NNN
    uint16_t value = opcode & 0x0FFFu;
    index= value;
}

void Chip8::OP_BNNN(){
    //PC value set to NNN plus value of V0
    pc= (opcode & 0x0FFFu) + registers[0x0u];
}

void Chip8::OP_CXKK(){
    //Set Vx=random num from (0, 255) ANDed with kk, stored in Vx
    uint8_t kk= opcode & 0x00FFu;
    uint8_t Vx= (opcode & 0x0F00u) >> 8;

    registers[Vx] = static_cast<uint8_t>(randByte(randGen)) & kk;
    //randByte is of unsigned int type, so we typecase it to be 0-255 since registers[Vx] is 8bits
}

void Chip8::OP_DXYN(){
    //Display n-byte sprite starting at memory location I at (Vx, Vy), Vf = collision

    //Note: memory location of sprite is stored at index register
    uint8_t Vx= (opcode & 0x0F00u) >> 8;
    uint8_t Vy= (opcode & 0x00F0u) >> 4;
    uint8_t height= opcode & 0x000Fu; 

    //Wrapping: Vx, Vy is from 0-255 but Screen is 64x32
    uint8_t xPos= registers[Vx] % DISPLAY_WIDHT;
    uint8_t yPos= registers[Vy] % DISPLAY_HEIGHT;
    registers[0xFu]= 0;

    for(unsigned int i=0 ; i<height; i++){
        uint8_t spriteByte = memory[index+i]; //Takes bytes like 0xF0, 0x90, 0x90... if '0' sprite taken
        for(unsigned int j=0; j<8; j++){  //Iterating through each pixel(bit in the byte)
            uint8_t spritePixel = spriteByte & (0x80u) >> j; //Extracts each pixel one by one 
            uint32_t * screenPixel = &display[(yPos+i) * DISPLAY_WIDHT + (xPos+j)];  //Pixels at screen to XOR

            if(spritePixel){
                if(*screenPixel == 0xFFFFFFFF){
                    //Sprite pixel and Screen pixel both are ON, colission
                    registers[0xFu]=1;
                }   
                //Screen pixel is OFF, so XOR with sprite Pixel thats ON
                *screenPixel ^= 0xFFFFFFFF; //Each pixel is uint32_t
            }

        }
    }
}

void Chip8::OP_EX9E(){
    //Skip next instruction if key with value of Vx is pressed
    uint8_t Vx= (opcode & 0x0F00u) >> 8;
    uint8_t key= registers[Vx];
    
    if(keypad[key])
        pc+=2;
}

void Chip8::OP_EXA1(){
    //Skip next instruction if key with value Vx is NOT pressed
    uint8_t Vx= (opcode & 0x0F00u) >> 8;
    uint8_t key= registers[Vx];
    
    if(!keypad[key])
        pc+=2;
}

void Chip8::OP_FX07(){
    //Set Vx= delay time value
    uint8_t Vx= (opcode & 0x0F00u) >> 8;
    registers[Vx]=delay;
}

void Chip8::OP_FX0A(){
    //Store value of key pressed in Vx
    uint8_t Vx= (opcode & 0x0F00u) >> 8;

    if (keypad[0]) registers[Vx] = 0;
    else if (keypad[1]) registers[Vx] = 1;
    else if (keypad[2]) registers[Vx] = 2;
    else if (keypad[3]) registers[Vx] = 3;
    else if (keypad[4]) registers[Vx] = 4;
    else if (keypad[5]) registers[Vx] = 5;
    else if (keypad[6]) registers[Vx] = 6;
    else if (keypad[7]) registers[Vx] = 7; 
    else if (keypad[8]) registers[Vx] = 8; 
    else if (keypad[9]) registers[Vx] = 9; 
    else if (keypad[10]) registers[Vx] = 10; 
    else if (keypad[11]) registers[Vx] = 11; 
    else if (keypad[12]) registers[Vx] = 12; 
    else if (keypad[13]) registers[Vx] = 13; 
    else if (keypad[14]) registers[Vx] = 14; 
    else if (keypad[15]) registers[Vx] = 15; 
    else
        pc-=2;  //If no input by user, just decrement program counter to previous instruction
}

void Chip8::OP_FX15(){
    //Set delay timer = Vx
    uint8_t Vx= (opcode & 0x0F00u) >> 8;
    delay= registers[Vx];
}

void Chip8::OP_FX18(){
    //Set sound timer = Vx
    uint8_t Vx= (opcode & 0x0F00u) >> 8;
    sound= registers[Vx];
}

void Chip8::OP_FX1E(){
    //Add I and Vx and store in I
    uint8_t Vx= (opcode & 0x0F00u) >> 8;
    index+= registers[Vx];
}

void Chip8::OP_FX29(){
    //Set I to location of sprite for digit Vx
    uint8_t Vx= (opcode & 0x0F00u) >> 8;
    uint8_t digit = registers[Vx];

    index= FONT_ADDRESS + (digit * 5);
}

void Chip8::OP_FX33(){
    ///Store BCD of Vx at memory locations I, I+1, I+2
    uint8_t Vx= (opcode & 0x0F00u) >> 8;
    uint8_t value = registers[Vx];

    for(int i=2; i>=0; i--){
        uint8_t store = value % 10u;
        memory[index+i] = store;
        value/=10;
    }
}

void Chip8::OP_FX55(){
    //Store registers V0 to VX starting at location I
    uint8_t Vx= (opcode & 0x0F00u) >> 8;
    for(unsigned int i=0; i<=Vx; i++)
        memory[index+i]=registers[i];
}

void Chip8::OP_FX65(){
    //Read registers V0 to Vx starting at location I
    uint8_t Vx= (opcode & 0x0F00u) >> 8;
    for(unsigned int i=0; i<=Vx; i++)
        registers[i]= memory[index+i];
}

void Chip8::Cycle(){
    //Fetch, decode, execute
    opcode = (memory[pc] << 8) | memory[pc+1];
    pc+=2;  //Increment pc to next instruction

    
    switch (opcode & 0xF000u) {
        case 0x0000:
            switch (opcode & 0x00FFu) {
                case 0x00E0: OP_00E0(); break;
                case 0x00EE: OP_00EE(); break;
            }
            break;

        case 0x1000: OP_1NNN(); break;
        case 0x2000: OP_2NNN(); break;
        case 0x3000: OP_3XKK(); break;
        case 0x4000: OP_4XKK(); break;
        case 0x5000: OP_5XY0(); break;
        case 0x6000: OP_6XKK(); break;
        case 0x7000: OP_7XKK(); break;

        case 0x8000:
            switch (opcode & 0x000Fu) {
                case 0x0000: OP_8XY0(); break;
                case 0x0001: OP_8XY1(); break;
                case 0x0002: OP_8XY2(); break;
                case 0x0003: OP_8XY3(); break;
                case 0x0004: OP_8XY4(); break;
                case 0x0005: OP_8XY5(); break;
                case 0x0006: OP_8XY6(); break;
                case 0x0007: OP_8XY7(); break;
                case 0x000E: OP_8XYE(); break;
            }
            break;

        case 0x9000: OP_9XY0(); break;
        case 0xA000: OP_ANNN(); break;
        case 0xB000: OP_BNNN(); break;
        case 0xC000: OP_CXKK(); break;
        case 0xD000: OP_DXYN(); break;

        case 0xE000:
            switch (opcode & 0x00FFu) {
                case 0x009E: OP_EX9E(); break;
                case 0x00A1: OP_EXA1(); break;
            }
            break;

        case 0xF000:
            switch (opcode & 0x00FFu) {
                case 0x0007: OP_FX07(); break;
                case 0x000A: OP_FX0A(); break;
                case 0x0015: OP_FX15(); break;
                case 0x0018: OP_FX18(); break;
                case 0x001E: OP_FX1E(); break;
                case 0x0029: OP_FX29(); break;
                case 0x0033: OP_FX33(); break;
                case 0x0055: OP_FX55(); break;
                case 0x0065: OP_FX65(); break;
            }
            break;
    }

    //Decrememnt delay timer if set
    if(delay>0)
        --delay;
    
    //Decrement sound timer if set
    if(sound>0)
        --sound;
}

