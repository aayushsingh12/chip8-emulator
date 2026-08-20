#define SDL_MAIN_HANDLED
#include "chip8.h"
#include "platform.h"
#include <iostream>
#include <chrono>
#include <string>

const unsigned int DISPLAY_WIDHT = 64;
const unsigned int DISPLAY_HEIGHT = 32;


int main(int argc, char const *argv[]) {
    /*
    ROM load test

    chip8.loadRom("roms/IBM Logo.ch8");
    for(int i=0; i<40; i++){
        chip8.Cycle();
        std::cout<<"Cycle: "<<i
                <<" | PC: 0x" <<std::hex<<chip8.pc
                <<" | Opcode: 0x"<<chip8.opcode
                <<" | I: 0x"<<chip8.index
                <<std::endl;
    }
    */
    
    //We will take inputs for scale of video, delay between clock cycles, and ROM

    if(argc!=4){
        std::cerr<<"Wrong number of arguments passed. Usage:"<< argv[0] << " Scale Delay ROM\n"<<std::endl;
        std::exit(EXIT_FAILURE);
    }
    
    int videoScale= std::stoi(argv[1]);
    int cycleDelay= std::stoi(argv[2]);
    char const* filename= argv[3];

    Platform platform("CHIP-8 Emulator", DISPLAY_WIDHT*videoScale, DISPLAY_HEIGHT * videoScale, DISPLAY_WIDHT, DISPLAY_HEIGHT);

    Chip8 chip8;
    chip8.loadRom(filename);

    int videoPitch= sizeof(chip8.display[0]) * DISPLAY_WIDHT;   
    //Pitch is just by how much are we shifting. 4bytes/pixel * 64pixels = 256 bytes

    auto lastCycleTime = std::chrono::high_resolution_clock::now(); //Storing last cycle time based on current time on clock
    bool quit= false;

    while(!quit){
        quit=platform.ProcessInput(chip8.keypad);   //Polling for input events every cycle

        auto currentTime = std::chrono::high_resolution_clock::now();
        float dt = std::chrono::duration<float, std::chrono::milliseconds::period>(currentTime - lastCycleTime).count();
        //Imp: Stores time difference. Once dt has passed, ie. its more that cycleDelay, we run Cycle() and Update(), fetching, decoding and executing opcode, and then updating whats displayed on the window

        if(dt>cycleDelay){  //Only run the loop if enough time has passed since last cycle, specified by user
            lastCycleTime = currentTime;
            chip8.Cycle();
            platform.Update(chip8.display, videoPitch); //Pushes whatever is in display[] onto the screen
        }
    }

    return 0;
}
