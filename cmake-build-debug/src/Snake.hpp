#ifndef SNAKE_HPP
#define SNAKE_HPP

#include <deque>
#include <SDL.h>
#include "Config.hpp"

namespace SnakeGame {

    using Config::Direction;

    class Snake {
    public:
        Snake(int startX, int startY, int cellSize, int initialLength);

        void move();
        void draw(SDL_Renderer* renderer) const;
        void changeDirection(Direction newDirection);
        void grow();

        [[nodiscard]] bool checkFoodCollision(const SDL_Point& foodPos) const;
        [[nodiscard]] bool checkSelfCollision() const;
        [[nodiscard]] bool checkWallCollision(int screenWidth, int screenHeight) const;

        [[nodiscard]] const std::deque<SDL_Point>& getBody() const;
        [[nodiscard]] SDL_Point getHeadPosition() const;
        [[nodiscard]] int getCellSize() const { return cellSize; }
        [[nodiscard]] Direction getCurrentDirection() const;

    private:
        std::deque<SDL_Point> body;
        Direction currentDirection;
        Direction nextDirection;
        bool growing;
        int cellSize;
    };

}

#endif