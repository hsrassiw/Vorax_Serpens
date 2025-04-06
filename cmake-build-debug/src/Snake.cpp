#include "Snake.hpp"
#include "Config.hpp"

Snake::Snake(int startX, int startY, int cellSize, int initialLength)
        : direction(Direction::RIGHT), cellSize(cellSize), directionChangedThisTick(false)
{
    for (int i = 0; i < initialLength; ++i) {
        body.push_back({startX - i * cellSize, startY});
    }
}

void Snake::move() {
    if (body.empty()) return;

    SDL_Point newHead = body.front();

    switch (direction) {
        case Direction::UP:    newHead.y -= cellSize; break;
        case Direction::DOWN:  newHead.y += cellSize; break;
        case Direction::LEFT:  newHead.x -= cellSize; break;
        case Direction::RIGHT: newHead.x += cellSize; break;
    }

    body.insert(body.begin(), newHead);

    body.pop_back();

    directionChangedThisTick = false;
}

void Snake::draw(SDL_Renderer* renderer) const {
    SDL_SetRenderDrawColor(renderer, Config::SNAKE_COLOR.r, Config::SNAKE_COLOR.g, Config::SNAKE_COLOR.b, Config::SNAKE_COLOR.a);
    for (const auto& segment : body) {
        SDL_Rect rect = {segment.x, segment.y, cellSize, cellSize};
        SDL_RenderFillRect(renderer, &rect);
    }
}

void Snake::changeDirection(Direction newDirection) {
    if (directionChangedThisTick) return;

    bool changingDirection = false;
    if ((direction == Direction::UP    && newDirection != Direction::DOWN)  ||
        (direction == Direction::DOWN  && newDirection != Direction::UP)    ||
        (direction == Direction::LEFT  && newDirection != Direction::RIGHT) ||
        (direction == Direction::RIGHT && newDirection != Direction::LEFT))
    {
        if (direction != newDirection) {
            direction = newDirection;
            changingDirection = true;
        }
    }
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
    if (!body.empty()) {
        body.push_back(body.back());
    }
}

const std::vector<SDL_Point>& Snake::getBody() const {
    return body;
}

SDL_Point Snake::getHeadPosition() const {
    return body.empty() ? SDL_Point{0, 0} : body.front();
}