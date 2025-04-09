#include "Snake.hpp"
#include "Config.hpp"
#include <iostream>

namespace SnakeGame {

    Snake::Snake(int startX, int startY, int size, int initialLength)
            : currentDirection(Direction::RIGHT),
              nextDirection(Direction::RIGHT),
              cellSize(size),
              growing(false)
    {
        for (int i = 0; i < initialLength; ++i) {
            body.push_back({startX - i * cellSize, startY});
        }
        if (body.empty()) {
            body.push_back({startX, startY});
        }
    }

    void Snake::move() {
        if (body.empty()) return;

        bool isOpposite = false;
        if ((currentDirection == Direction::UP && nextDirection == Direction::DOWN) ||
            (currentDirection == Direction::DOWN && nextDirection == Direction::UP) ||
            (currentDirection == Direction::LEFT && nextDirection == Direction::RIGHT) ||
            (currentDirection == Direction::RIGHT && nextDirection == Direction::LEFT)) {
            isOpposite = true;
        }

        if (!isOpposite) {
            currentDirection = nextDirection;
        }

        SDL_Point newHead = body.front();
        switch (currentDirection) {
            case Direction::UP:    newHead.y -= cellSize; break;
            case Direction::DOWN:  newHead.y += cellSize; break;
            case Direction::LEFT:  newHead.x -= cellSize; break;
            case Direction::RIGHT: newHead.x += cellSize; break;
        }

        body.push_front(newHead);

        if (growing) {
            growing = false;
        } else {
            body.pop_back();
        }
    }

    void Snake::draw(SDL_Renderer* renderer) const {
        if (!renderer) return;
        SDL_SetRenderDrawColor(renderer, Config::SNAKE_COLOR.r, Config::SNAKE_COLOR.g, Config::SNAKE_COLOR.b, Config::SNAKE_COLOR.a);
        for (const auto& segment : body) {
            SDL_Rect rect = {segment.x, segment.y, cellSize, cellSize};
            SDL_RenderFillRect(renderer, &rect);
        }
    }

    void Snake::changeDirection(Direction newDirection) {
        nextDirection = newDirection;
    }

    bool Snake::checkFoodCollision(const SDL_Point& foodPos) const {
        if (body.empty()) return false;
        return body.front().x == foodPos.x && body.front().y == foodPos.y;
    }

    bool Snake::checkSelfCollision() const {
        if (body.size() < 2) return false;

        const SDL_Point& head = body.front();
        for (size_t i = 1; i < body.size(); ++i) {
            if (head.x == body[i].x && head.y == body[i].y) {
                return true;
            }
        }
        return false;
    }

    bool Snake::checkWallCollision(int screenWidth, int screenHeight) const {
        if (body.empty()) return false;
        const SDL_Point& head = body.front();
        return (head.x < 0 || head.x >= screenWidth || head.y < 0 || head.y >= screenHeight);
    }

    void Snake::grow() {
        growing = true;
    }

    const std::deque<SDL_Point>& Snake::getBody() const {
        return body;
    }

    SDL_Point Snake::getHeadPosition() const {
        return body.empty() ? SDL_Point{-1, -1} : body.front();
    }

    Direction Snake::getCurrentDirection() const {
        return currentDirection;
    }

}