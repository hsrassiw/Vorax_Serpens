#include <SDL.h>
#include <SDL_ttf.h>
#include "Renderer.hpp"
#include "Config.hpp"
#include "Food.hpp"
#include "Game.hpp"

int main(int argc, char* argv[]) {
    SDL_Init(SDL_INIT_VIDEO);
    TTF_Init();
    SDL_Window* window = SDL_CreateWindow("Vorax Serpens", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                          Config :: SCREEN_WIDTH, Config :: SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    Renderer renderer(window);
    Game game(Config :: SCREEN_WIDTH, Config :: SCREEN_HEIGHT, Config :: CELL_SIZE);

    bool quit = false;
    SDL_Event event;
    while (!quit) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) quit = true;
            game.handleInput(event);
        }

        game.update();
        game.render(renderer);

        SDL_Delay(110);
    }

    SDL_DestroyWindow(window);
    TTF_Quit();
    SDL_Quit();
    return 0;
}