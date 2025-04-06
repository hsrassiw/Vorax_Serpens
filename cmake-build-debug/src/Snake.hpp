#ifndef SNAKE_HPP
#define SNAKE_HPP

#include <vector>
#include <SDL.h>

class Snake {
public:
    enum class Direction { UP, DOWN, LEFT, RIGHT };

    Snake(int startX, int startY, int cellSize, int initialLength);

    void move();
    void draw(SDL_Renderer* renderer) const;
    void changeDirection(Direction newDirection);
    void grow();

    [[nodiscard]] bool checkFoodCollision(const SDL_Point& foodPos) const;
    [[nodiscard]] bool checkSelfCollision() const;
    [[nodiscard]] bool checkWallCollision(int screenWidth, int screenHeight) const;

    [[nodiscard]] const std::vector<SDL_Point>& getBody() const;
    [[nodiscard]] SDL_Point getHeadPosition() const;
    [[nodiscard]] int getSize() const { return cellSize; }


private:
    std::vector<SDL_Point> body;
    Direction direction;
    int cellSize;
    bool directionChangedThisTick;
};

#endif