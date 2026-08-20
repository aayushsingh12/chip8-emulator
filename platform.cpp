#define SDL_MAIN_HANDLED

#include <SDL2/SDL.h>
#include <iostream>
#include "platform.h"

Platform::Platform(char const *title, int windowWidth, int windowHeight, int textureWidth, int textureHeight)
{
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        std::cerr << "SDL could not be initialised: " << SDL_GetError() << std::endl;
        return;
    }

    /*
    Window is the OS window itself, Renderer is the drawing engine, bound to that window.
    Basically, its the paintbruh that draws onto the window. The flag used in the renderer tells
    SDL to use the GPU for drawing. Texture is the actual pixel data, where the display
    buffer is essentially stored. The renderer takes this and draws onto the display. So,
    we can store stuff in this buffer and tell the renderer to draw to the screen. We essentially
    are copying the display[] buffer (in CHIP-8 class) and putting it in the texture buffer every
    frame.
    */

    window = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, windowWidth, windowHeight, SDL_WINDOW_SHOWN);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    texture = SDL_CreateTexture(
        renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, textureWidth, textureHeight);
}

Platform::~Platform()
{
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void Platform::Update(void const *buffer, int pitch)
{
    SDL_UpdateTexture(texture, nullptr, buffer, pitch);  // Pitch is essentially how many bytes to move forward. This is why display is a 1D array, not a 2D grid
    SDL_RenderClear(renderer);                           // Clears the renderer
    SDL_RenderCopy(renderer, texture, nullptr, nullptr); // Draws texture onto target
    SDL_RenderPresent(renderer);                         // Makes the frame visible on screen
}

bool Platform::ProcessInput(uint8_t *keys)
{
    bool quit = false;

    SDL_Event event;

    while (SDL_PollEvent(&event))
    {
        switch (event.type)
        {
        case SDL_QUIT:
        {
            quit = true;
        }
        break;

        case SDL_KEYDOWN:
        {
            switch (event.key.keysym.sym)
            {
            case SDLK_ESCAPE:
            {
                quit = true;
            }
            break;

            case SDLK_x:
            {
                keys[0] = 1;
            }
            break;

            case SDLK_1:
            {
                keys[1] = 1;
            }
            break;

            case SDLK_2:
            {
                keys[2] = 1;
            }
            break;

            case SDLK_3:
            {
                keys[3] = 1;
            }
            break;

            case SDLK_q:
            {
                keys[4] = 1;
            }
            break;

            case SDLK_w:
            {
                keys[5] = 1;
            }
            break;

            case SDLK_e:
            {
                keys[6] = 1;
            }
            break;

            case SDLK_a:
            {
                keys[7] = 1;
            }
            break;

            case SDLK_s:
            {
                keys[8] = 1;
            }
            break;

            case SDLK_d:
            {
                keys[9] = 1;
            }
            break;

            case SDLK_z:
            {
                keys[0xA] = 1;
            }
            break;

            case SDLK_c:
            {
                keys[0xB] = 1;
            }
            break;

            case SDLK_4:
            {
                keys[0xC] = 1;
            }
            break;

            case SDLK_r:
            {
                keys[0xD] = 1;
            }
            break;

            case SDLK_f:
            {
                keys[0xE] = 1;
            }
            break;

            case SDLK_v:
            {
                keys[0xF] = 1;
            }
            break;
            }
        }
        break;

        case SDL_KEYUP:
        {
            switch (event.key.keysym.sym)
            {
            case SDLK_x:
            {
                keys[0] = 0;
            }
            break;

            case SDLK_1:
            {
                keys[1] = 0;
            }
            break;

            case SDLK_2:
            {
                keys[2] = 0;
            }
            break;

            case SDLK_3:
            {
                keys[3] = 0;
            }
            break;

            case SDLK_q:
            {
                keys[4] = 0;
            }
            break;

            case SDLK_w:
            {
                keys[5] = 0;
            }
            break;

            case SDLK_e:
            {
                keys[6] = 0;
            }
            break;

            case SDLK_a:
            {
                keys[7] = 0;
            }
            break;

            case SDLK_s:
            {
                keys[8] = 0;
            }
            break;

            case SDLK_d:
            {
                keys[9] = 0;
            }
            break;

            case SDLK_z:
            {
                keys[0xA] = 0;
            }
            break;

            case SDLK_c:
            {
                keys[0xB] = 0;
            }
            break;

            case SDLK_4:
            {
                keys[0xC] = 0;
            }
            break;

            case SDLK_r:
            {
                keys[0xD] = 0;
            }
            break;

            case SDLK_f:
            {
                keys[0xE] = 0;
            }
            break;

            case SDLK_v:
            {
                keys[0xF] = 0;
            }
            break;
            }
        }
        break;
        }
    }

    return quit;
}
