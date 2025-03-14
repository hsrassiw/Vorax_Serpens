#include "Snake.hpp"

Snake::Snake(int x, int y, int size, int initialLength)
    : direction(Direction::RIGHT), size(size) {
    for (int i = 0; i < initialLength; ++i) {
        body.push_back({x - i * size, y});
    }
}

void Snake::move() {
    SDL_Point head = body.front();
    switch (direction) {
        case Direction::UP:    head.y -= size; break;
        case Direction::DOWN:  head.y += size; break;
        case Direction::LEFT:  head.x -= size; break;
        case Direction::RIGHT: head.x += size; break;
    }
    body.insert(body.begin(), head);
    body.pop_back();
}

void Snake::draw(SDL_Renderer* renderer) const {
    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
    for (const auto& segment : body) {
        SDL_Rect rect = {segment.x, segment.y, size, size};
        SDL_RenderFillRect(renderer, &rect);
    }
}

void Snake::changeDirection(Direction newDirection) {
    if ((direction == Direction::UP && newDirection == Direction::DOWN) ||
        (direction == Direction::DOWN && newDirection == Direction::UP) ||
        (direction == Direction::LEFT && newDirection == Direction::RIGHT) ||
        (direction == Direction::RIGHT && newDirection == Direction::LEFT)) {
        return;
        }
    direction = newDirection;
}

std::vector<SDL_Point> Snake::getBody() const {
    return body;
}
