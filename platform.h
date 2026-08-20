#pragma once

#include <SDL2/SDL.h>
#include <cstdint>

class Platform{
public:
    Platform(char const * title, int windowWidth, int windowHeight, int textureWidth, int textureHeight);
    ~Platform();
    void Update(void const * buffer, int pitch);    //Copy display array to texture
    bool ProcessInput(uint8_t * keys);  //Handling input by user

private:
    SDL_Window * window{};
    SDL_Renderer * renderer{};
    SDL_Texture * texture{};
};
