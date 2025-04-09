#ifndef GAME_HPP
#define GAME_HPP

#include "Snake.hpp"
#include "Food.hpp"
#include "Renderer.hpp"
#include "Config.hpp"
#include <SDL.h>
#include <string>

namespace SnakeGame {

    enum class GameState {
        Playing,
        Paused,
        GameOver
    };

    class Game {
    public:
        Game(int screenWidth, int screenHeight, int cellSize);

        void handleInput(const SDL_Event& event);
        void update();
        void render(Renderer& renderer) const;
        void runFrame(float deltaTime);

        [[nodiscard]] bool isGameOver() const;
        [[nodiscard]] bool isPaused() const;
        [[nodiscard]] GameState getCurrentState() const;
        [[nodiscard]] int getScore() const { return score; }
        [[nodiscard]] int getHighScore() const { return highScore; }

        void reset();

    private:
        int screenWidth;
        int screenHeight;
        int cellSize;

        Snake snake;
        Food food;

        GameState currentState;
        int score;
        int highScore;
        Uint32 moveInterval;

        float timeAccumulator;

        SDL_Point calculateStartPosition() const;
        void increaseSpeed();
        void placeFood();

        int loadHighScore();
        void saveHighScore() const;
    };

}

#endif