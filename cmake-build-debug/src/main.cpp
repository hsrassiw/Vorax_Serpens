#include <SDL.h>
#include <SDL_ttf.h>
#include "Renderer.hpp"
#include "Snake.hpp"
#include "Food.hpp"
#include "Game.hpp"

int main(int argc, char* argv[]) {
    SDL_Init(SDL_INIT_VIDEO);
    TTF_Init();
    SDL_Window* window = SDL_CreateWindow("Vorax Serpens", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                          800, 600, SDL_WINDOW_SHOWN);
    Renderer renderer(window);
    Game game(800, 600, 20);

    bool quit = false;
    SDL_Event event;
    while (!quit) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) quit = true;
            game.handleInput(event);
        }

        game.update();

        game.render(renderer);

        if (game.isGameOver()) {
            SDL_Delay(200);
            quit = true;
        }

        SDL_Delay(100);
    }

    SDL_DestroyWindow(window);
    TTF_Quit();
    SDL_Quit();
    return 0;
}