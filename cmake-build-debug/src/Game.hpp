#ifndef GAME_HPP
#define GAME_HPP

#include "Snake.hpp"
#include "Food.hpp"
#include "Renderer.hpp"
#include <SDL.h>
#include <string>

class Game {
public:
    Game(int screenWidth, int screenHeight, int cellSize);

    void handleInput(const SDL_Event& event);
    void update();
    void render(Renderer& renderer) const;

    [[nodiscard]] bool isGameOver() const;
    [[nodiscard]] int getScore() const { return score; }
    [[nodiscard]] int getHighScore() const { return highScore; }
    [[nodiscard]] Uint32 getMoveInterval() const;

    void reset();

private:
    int screenWidth;
    int screenHeight;
    int cellSize;

    Snake snake;
    Food food;

    bool gameOver;
    int score;
    int highScore;
    Uint32 moveInterval;

    SDL_Point calculateStartPosition() const;
    void increaseSpeed();
};

#endif