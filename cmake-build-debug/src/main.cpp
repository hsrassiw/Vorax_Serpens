#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL_image.h>
#include <iostream>
#include <stdexcept>
#include <string>
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
    int imgFlags = IMG_INIT_PNG | IMG_INIT_JPG;
    if (!(IMG_Init(imgFlags) & imgFlags)) {
        std::cerr << "SDL_image could not initialize! IMG_Error: " << IMG_GetError() << std::endl;
        SDL_Quit();
        return 1;
    }
    if (TTF_Init() == -1) {
        std::cerr << "SDL_ttf could not initialize! TTF_Error: " << TTF_GetError() << std::endl;
        IMG_Quit();
        SDL_Quit();
        return 1;
    }

    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        std::cerr << "SDL_mixer could not initialize! Mix_Error: " << Mix_GetError() << std::endl;
    }

    auto windowDeleter = [](SDL_Window* w){ if(w) SDL_DestroyWindow(w); };
    std::unique_ptr<SDL_Window, decltype(windowDeleter)> window(nullptr, windowDeleter);
    std::unique_ptr<Renderer> renderer = nullptr;
    std::unique_ptr<Game> game = nullptr;

    try {
        window.reset(SDL_CreateWindow(
                "Vorax Serpens",
                SDL_WINDOWPOS_CENTERED,
                SDL_WINDOWPOS_CENTERED,
                Config::SCREEN_WIDTH,
                Config::SCREEN_HEIGHT,
                SDL_WINDOW_SHOWN
        ));
        if (!window) {
            std::string errorMsg = "Window creation failed: ";
            errorMsg += SDL_GetError();
            throw std::runtime_error(errorMsg);
        }

        renderer = std::make_unique<Renderer>(window.get(), Config::FONT_PATH, Config::FONT_SIZE);
        game = std::make_unique<Game>(Config::SCREEN_WIDTH, Config::SCREEN_HEIGHT, Config::CELL_SIZE, *renderer);

        bool userQuit = false;
        SDL_Event event;
        Uint64 lastFrameTicks = SDL_GetPerformanceCounter();

        while (!userQuit) {
            Uint64 currentFrameTicks = SDL_GetPerformanceCounter();
            Uint64 frequency = SDL_GetPerformanceFrequency();
            if (frequency == 0) { frequency = 1; }
            float deltaTime = static_cast<float>(currentFrameTicks - lastFrameTicks) / frequency;
            lastFrameTicks = currentFrameTicks;

            while (SDL_PollEvent(&event)) {
                if (event.type == SDL_QUIT) {
                    userQuit = true;
                    break;
                }
                game->handleInput(event);
            }

            if (userQuit) {
                break;
            }

            if (game->didQuit()) {
                userQuit = true;
            }

            game->runFrame(deltaTime);
            game->render(*renderer);

        }

    } catch (const std::runtime_error& e) {
        std::cerr << "Runtime Error: " << e.what() << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    } catch (...) {
        std::cerr << "An unknown error occurred." << std::endl;
    }

    std::cout << "Cleaning up..." << std::endl;
    game.reset();
    renderer.reset();
    window.reset();
    Mix_CloseAudio();
    Mix_Quit();
    TTF_Quit();
    IMG_Quit();
    SDL_Quit();
    std::cout << "Cleanup complete. Exiting." << std::endl;

    return 0;
}