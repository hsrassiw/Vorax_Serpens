#include "Food.hpp"

Food::Food(int size) : position(), size(size), rng(std::random_device{}()) {}

void Food::generate(int screenWidth, int screenHeight, const std::vector<SDL_Point>& snakeBody) {
    std::uniform_int_distribution<int> distX(0, (screenWidth / size) - 1);
    std::uniform_int_distribution<int> distY(0, (screenHeight / size) - 1);

    bool validPosition;
    do {
        position.x = distX(rng) * size;
        position.y = distY(rng) * size;
        validPosition = true;

        for (const auto& segment : snakeBody) {
            if (position.x == segment.x && position.y == segment.y) {
                validPosition = false;
                break;
            }
        }
    } while (!validPosition);
}

void Food::draw(SDL_Renderer* renderer) const {
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    SDL_Rect rect = {position.x, position.y, size, size};
    SDL_RenderFillRect(renderer, &rect);
}

SDL_Point Food::getPosition() const {
    return position;
}
