#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <SDL.h>

namespace Config {
    constexpr int SCREEN_WIDTH = 800;
    constexpr int SCREEN_HEIGHT = 600;
    constexpr int CELL_SIZE = 20;


    constexpr SDL_Color BACKGROUND_COLOR = {0, 0, 0, 255};

    constexpr const char* FONT_PATH = "D:/Clion/Vorax_Serpens/assets/fonts/Arial.ttf";
}

#endif // CONFIG_HPP
