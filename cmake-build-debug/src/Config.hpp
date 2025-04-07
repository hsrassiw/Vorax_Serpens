#ifndef VORAX_SERPENS_CONFIG_HPP
#define VORAX_SERPENS_CONFIG_HPP

#include <SDL.h>
#include <string>

namespace Config {

    constexpr int SCREEN_WIDTH = 1000;
    constexpr int SCREEN_HEIGHT = 800;
    constexpr int CELL_SIZE = 20;

    constexpr int DEFAULT_SNAKE_LENGTH = 3;
    constexpr int INITIAL_SNAKE_SPEED_DELAY_MS = 150;
    constexpr int SPEED_INCREMENT_MS = 4;
    constexpr int MIN_MOVE_INTERVAL_MS = 50;

    constexpr SDL_Color SNAKE_COLOR = {0, 255, 0, 255};
    constexpr SDL_Color FOOD_COLOR = {255, 0, 0, 255};
    constexpr SDL_Color BACKGROUND_COLOR = {30, 30, 30, 255};
    constexpr SDL_Color TEXT_COLOR = {255, 255, 255, 255};
    constexpr SDL_Color GAMEOVER_TEXT_COLOR = {255, 50, 50, 255};

    const std::string FONT_PATH = "assets/fonts/arial.ttf";
    constexpr int FONT_SIZE = 24;

    enum class Controls { UP, DOWN, LEFT, RIGHT, RESTART, NONE };

    Controls handleInput(const SDL_Event& event);

}

#endif