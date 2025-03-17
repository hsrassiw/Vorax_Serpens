#ifndef VORAX_SERPENS_CONFIG_HPP
#define VORAX_SERPENS_CONFIG_HPP

#include <SDL.h>

namespace Config {


    constexpr int SCREEN_WIDTH = 1000;
    constexpr int SCREEN_HEIGHT = 800;
    constexpr int CELL_SIZE = 20;
    constexpr int DEFAULT_SNAKE_LENGTH = 3;
    constexpr int INITIAL_SNAKE_SPEED = 100;
    constexpr int SPEED_INCREMENT = 5;

    constexpr SDL_Color SNAKE_COLOR = {0, 255, 0, 255};
    constexpr SDL_Color FOOD_COLOR = {255, 0, 0, 255};
    constexpr SDL_Color BACKGROUND_COLOR = {0, 0, 0, 255};
    constexpr SDL_Color TEXT_COLOR = {255, 255, 255, 255};


    enum class Controls { UP, DOWN, LEFT, RIGHT, RESTART, NONE };

    Controls handleInput(const SDL_Event& event);

}

#endif