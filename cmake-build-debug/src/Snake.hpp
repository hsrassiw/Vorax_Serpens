#ifndef SNAKE_HPP
#define SNAKE_HPP

#include <vector>
#include <SDL.h>

class Snake {
public:
    enum class Direction { UP, DOWN, LEFT, RIGHT };

    Snake(int x, int y, int size, int initialLength);
    void move();
    void draw(SDL_Renderer* renderer) const;
    void changeDirection(Direction newDirection);
    [[nodiscard]] bool checkCollision(const SDL_Point& foodPos) const;
    [[nodiscard]] bool checkSelfCollision() const;
    [[nodiscard]] bool checkWallCollision(int screenWidth, int screenHeight) const;
    [[nodiscard]] std::vector<SDL_Point> getBody() const;
    [[nodiscard]] SDL_Point getHeadPosition() const;
    void grow();
    int getSize() const { return size; }


private:
    std::vector<SDL_Point> body;
    Direction direction;
    int size;
};

#endif
