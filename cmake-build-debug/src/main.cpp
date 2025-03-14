#include <SDL.h>
#include <iostream>
#include "Renderer.hpp"

int main(int argc, char* argv[]) {

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL Initialization failed: " << SDL_GetError() << std::endl;
        return 1;
    }


    SDL_Window* window = SDL_CreateWindow("Vorax Serpens", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                          800, 600, SDL_WINDOW_SHOWN);
    if (!window) {
        std::cerr << "Window creation failed: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return 1;
    }


    Renderer renderer(window);
    if (!renderer.getRenderer()) {
        std::cerr << "Renderer creation failed!" << std::endl;
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }


    bool quit = false;
    SDL_Event event;
    while (!quit) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) quit = true;
        }

        renderer.clear();
        renderer.present();
        SDL_Delay(16);
    }


    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
