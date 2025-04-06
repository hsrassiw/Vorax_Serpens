#include "Config.hpp"

namespace Config {

    Controls handleInput(const SDL_Event& event) {
        if (event.type == SDL_KEYDOWN) {
            switch (event.key.keysym.sym) {
                case SDLK_w: case SDLK_UP:    return Controls::UP;
                case SDLK_s: case SDLK_DOWN:  return Controls::DOWN;
                case SDLK_a: case SDLK_LEFT:  return Controls::LEFT;
                case SDLK_d: case SDLK_RIGHT: return Controls::RIGHT;
                case SDLK_SPACE:              return Controls::RESTART;
                default:                      return Controls::NONE;
            }
        } else if (event.type == SDL_KEYUP) {

        }

        return Controls::NONE;
    }

}