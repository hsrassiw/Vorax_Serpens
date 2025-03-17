#ifndef GAME_HPP
#define GAME_HPP

#include "Snake.hpp"
#include "Food.hpp"
#include "Renderer.hpp"


class Game {
public:
    Game(int w, int h, int size);
    void handleInput(const SDL_Event& event);
    void update();
    void render(const Renderer& renderer) const;
    [[nodiscard]] bool isGameOver() const;
    void reset();


private:
    Snake snake;
    Food food;
    int screenWidth, screenHeight;
    bool gameOver;
    int score;
    int highScore{};
};

#endif
