#include "Snake.hpp"

Snake::Snake(int x, int y, int size, int initialLength)
    : direction(Direction::RIGHT), size(size) {
    for (int i = 0; i < initialLength; ++i) {
        body.push_back({x - i * size, y});
    }
}

void Snake::draw(SDL_Renderer* renderer) const {
    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
    for (const auto& segment : body) {
        SDL_Rect rect = {segment.x, segment.y, size, size};
        SDL_RenderFillRect(renderer, &rect);
    }
}
std::vector<SDL_Point> Snake::getBody() const {
    return body;
}
