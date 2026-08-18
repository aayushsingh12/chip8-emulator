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

    randByte = std::uniform_int_distribution<uint8_t>(0, 255U); 

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
    for(unsigned int i=0; i<DISPLAY_WIDHT* DISPLAY_HEIGHT; i++)
        display[i]=0x00000000;
}

void Chip8::OP_00EE(){
    if(sp==0){
        std::cout<<"Invalid instruction; stack pointer at bottom"<<std::endl;
        return;
    }
    pc= stack[sp];
    sp--;
}

void Chip8::OP_1NNN(){
    
}
