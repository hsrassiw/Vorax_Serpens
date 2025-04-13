#include "Config.hpp"

namespace SnakeGame {
    namespace Config {

        ControlInput handleRawInput(const SDL_Event& event) {
            if (event.type == SDL_KEYDOWN) {
                switch (event.key.keysym.sym) {
                    case SDLK_w:
                    case SDLK_UP:
                        return ControlInput::UP;
                    case SDLK_s:
                    case SDLK_DOWN:
                        return ControlInput::DOWN;
                    case SDLK_a:
                    case SDLK_LEFT:
                        return ControlInput::LEFT;
                    case SDLK_d:
                    case SDLK_RIGHT:
                        return ControlInput::RIGHT;
                    case SDLK_SPACE:
                        return ControlInput::RESTART;
                    case SDLK_RETURN:
                    case SDLK_KP_ENTER:
                        return ControlInput::CONFIRM;
                    case SDLK_p:
                        return ControlInput::PAUSE;
                    case SDLK_ESCAPE:
                        return ControlInput::NONE;
                    default:
                        return ControlInput::NONE;
                }
            }
            return ControlInput::NONE;
        }

    }
}