#include <SDL.h>
#include <SDL_ttf.h>
#include <iostream>
#include <stdexcept>
#include "Config.hpp"
#include "Renderer.hpp"
#include "Game.hpp"

int main(int argc, char* argv[]) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
        return 1;
    }

    if (TTF_Init() == -1) {
        std::cerr << "SDL_ttf could not initialize! TTF_Error: " << TTF_GetError() << std::endl;
        SDL_Quit();
        return 1;
    }

    SDL_Window* window = nullptr;
    Renderer* renderer = nullptr;
    Game* game = nullptr;

    try {
        window = SDL_CreateWindow(
                "Vorax Serpens - Timed Update",
                SDL_WINDOWPOS_CENTERED,
                SDL_WINDOWPOS_CENTERED,
                Config::SCREEN_WIDTH,
                Config::SCREEN_HEIGHT,
                SDL_WINDOW_SHOWN
        );
        if (!window) {
            throw std::runtime_error("Window could not be created! SDL_Error: " + std::string(SDL_GetError()));
        }

        renderer = new Renderer(window, Config::FONT_PATH, Config::FONT_SIZE);
        game = new Game(Config::SCREEN_WIDTH, Config::SCREEN_HEIGHT, Config::CELL_SIZE);

        bool quit = false;
        SDL_Event event;

        Uint32 lastFrameTime = SDL_GetTicks();
        Uint32 timeAccumulator = 0;

        while (!quit) {
            Uint32 currentFrameTime = SDL_GetTicks();
            Uint32 deltaTime = currentFrameTime - lastFrameTime;
            lastFrameTime = currentFrameTime;

            while (SDL_PollEvent(&event)) {
                if (event.type == SDL_QUIT) {
                    quit = true;
                }
                game->handleInput(event);
            }

            timeAccumulator += deltaTime;

            Uint32 currentMoveInterval = game->getMoveInterval();

            if (!game->isGameOver()) {
                while (timeAccumulator >= currentMoveInterval) {
                    game->update();
                    timeAccumulator -= currentMoveInterval;

                    currentMoveInterval = game->getMoveInterval();
                    if (currentMoveInterval == 0) break;
                }
            } else {
                timeAccumulator = 0;
            }

            game->render(*renderer);

            SDL_Delay(1);

        }

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }

    std::cout << "Cleaning up..." << std::endl;
    delete game;
    delete renderer;
    if (window) {
        SDL_DestroyWindow(window);
    }
    TTF_Quit();
    SDL_Quit();
    std::cout << "Cleanup complete. Exiting." << std::endl;

    return 0;
}