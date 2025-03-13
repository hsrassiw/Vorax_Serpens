#include <SDL.h>
#include <SDL_ttf.h>
#include <iostream>
#include <memory>
#include "Game.hpp"
#include "Config.hpp"
#include "Renderer.hpp"

int main(int argc, char* argv[]) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0 || TTF_Init() == -1) {
        std::cerr << "Initialization failed: " << SDL_GetError() << std::endl;
        return 1;
    }

    std::unique_ptr<SDL_Window, decltype(&SDL_DestroyWindow)> window(
        SDL_CreateWindow("Vorax_Serpens", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                         Config::SCREEN_WIDTH, Config::SCREEN_HEIGHT, SDL_WINDOW_SHOWN),
        SDL_DestroyWindow
    );

    if (!window) {
        std::cerr << "Window creation failed: " << SDL_GetError() << std::endl;
        TTF_Quit();
        SDL_Quit();
        return 1;
    }

    return 0;
}
