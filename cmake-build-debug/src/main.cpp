#include <SDL.h>
#include <SDL_ttf.h>
#include <iostream>
#include <stdexcept>
#include <memory>
#include <algorithm>
#include "Config.hpp"
#include "Renderer.hpp"
#include "Game.hpp"

using namespace SnakeGame;

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

    auto windowDeleter = [](SDL_Window* w){ if(w) SDL_DestroyWindow(w); };

    std::unique_ptr<SDL_Window, decltype(windowDeleter)> window(nullptr, windowDeleter);
    std::unique_ptr<Renderer> renderer = nullptr;
    std::unique_ptr<Game> game = nullptr;

    try {
        window.reset(SDL_CreateWindow(
                "Vorax Serpens Refactored FSM",
                SDL_WINDOWPOS_CENTERED,
                SDL_WINDOWPOS_CENTERED,
                Config::SCREEN_WIDTH,
                Config::SCREEN_HEIGHT,
                SDL_WINDOW_SHOWN
        ));
        if (!window) {
            throw std::runtime_error("Window could not be created! SDL_Error: " + std::string(SDL_GetError()));
        }

        renderer = std::make_unique<Renderer>(window.get(), Config::FONT_PATH, Config::FONT_SIZE);

        game = std::make_unique<Game>(Config::SCREEN_WIDTH, Config::SCREEN_HEIGHT, Config::CELL_SIZE);

        bool quit = false;
        SDL_Event event;

        Uint64 lastFrameTicks = SDL_GetPerformanceCounter();

        while (!quit) {
            Uint64 currentFrameTicks = SDL_GetPerformanceCounter();
            Uint64 frequency = SDL_GetPerformanceFrequency();
            if (frequency == 0) {
                frequency = 1;
            }
            float deltaTime = static_cast<float>(currentFrameTicks - lastFrameTicks) / frequency;
            lastFrameTicks = currentFrameTicks;

            while (SDL_PollEvent(&event)) {
                if (event.type == SDL_QUIT) {
                    quit = true;
                }
                game->handleInput(event);
            }

            game->runFrame(deltaTime);

            game->render(*renderer);

        }

    } catch (const RendererError& e) {
        std::cerr << "Renderer Error: " << e.what() << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    } catch (...) {
        std::cerr << "An unknown error occurred." << std::endl;
    }

    std::cout << "Cleaning up..." << std::endl;
    game.reset();
    renderer.reset();
    window.reset();

    TTF_Quit();
    SDL_Quit();
    std::cout << "Cleanup complete. Exiting." << std::endl;

    return 0;
}