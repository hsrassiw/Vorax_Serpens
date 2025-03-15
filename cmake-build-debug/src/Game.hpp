#ifndef GAME_HPP
#define GAME_HPP

#include "Snake.hpp"
#include "Food.hpp"
#include "Renderer.hpp"
#include "Config.hpp"

class Game {
public:
    Game(int w, int h, int size);
    void handleInput(const SDL_Event& event);
    void update();
    void render(Renderer& renderer) const;
    bool isGameOver() const;

private:
    Snake snake;
    Food food;
    int screenWidth, screenHeight;
    bool gameOver;
};

#endif // GAME_HPP
