#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>
#include <iostream>
#include <algorithm>
#include "Config.hpp"
#include "Renderer.hpp"
#include "Game.hpp"

using namespace SnakeGame;

int main(int argc, char* argv[]) {

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0 ||
        !(IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG)) ||
        TTF_Init() == -1) {
        std::cerr << "SDL init failed: "
                  << SDL_GetError() << " | "
                  << IMG_GetError() << " | "
                  << TTF_GetError() << std::endl;
        return 1;
    }

    // Init SDL_mixer (optional)
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        std::cerr << "Warning: Mix_OpenAudio failed: " << Mix_GetError() << std::endl;
    }

    SDL_Window* window = SDL_CreateWindow("Snake Game",
                                          SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                          Config::SCREEN_WIDTH, Config::SCREEN_HEIGHT,
                                          SDL_WINDOW_SHOWN);

    if (!window) {
        std::cerr << "Window creation failed: " << SDL_GetError() << std::endl;
        Mix_Quit(); TTF_Quit(); IMG_Quit(); SDL_Quit();
        return 1;
    }

    Renderer renderer(window, Config::FONT_PATH, Config::FONT_SIZE);
    Game game(Config::SCREEN_WIDTH, Config::SCREEN_HEIGHT, Config::CELL_SIZE, renderer);

    bool running = true;
    SDL_Event event;
    Uint64 lastTick = SDL_GetPerformanceCounter();

    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) running = false;
            game.handleInput(event);
            if (game.didQuit()) running = false;
        }

        Uint64 now = SDL_GetPerformanceCounter();
        float dt = (float)(now - lastTick) / SDL_GetPerformanceFrequency();
        lastTick = now;

        game.runFrame(std::min(dt, 0.1f));
        game.render(renderer);
    }

    // Cleanup
    SDL_DestroyWindow(window);
    Mix_CloseAudio();
    Mix_Quit();
    TTF_Quit();
    IMG_Quit();
    SDL_Quit();
    return 0;
}
