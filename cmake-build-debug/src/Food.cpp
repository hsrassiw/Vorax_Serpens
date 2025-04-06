#include "Food.hpp"
#include "Config.hpp"

Food::Food(int size) : position{0, 0}, size(size), rng(std::random_device{}()) {}

void Food::generate(int screenWidth, int screenHeight, const std::vector<SDL_Point>& snakeBody) {
    int maxGridX = (screenWidth / size) - 1;
    int maxGridY = (screenHeight / size) - 1;

    std::uniform_int_distribution<int> distX(0, maxGridX);
    std::uniform_int_distribution<int> distY(0, maxGridY);

    bool validPosition;
    int potentialX, potentialY;
    do {
        potentialX = distX(rng) * size;
        potentialY = distY(rng) * size;
        validPosition = true;

        for (const auto& segment : snakeBody) {
            if (potentialX == segment.x && potentialY == segment.y) {
                validPosition = false;
                break;
            }
        }
    } while (!validPosition);

    position.x = potentialX;
    position.y = potentialY;
}

void Food::draw(SDL_Renderer* renderer) const {
    SDL_SetRenderDrawColor(renderer, Config::FOOD_COLOR.r, Config::FOOD_COLOR.g, Config::FOOD_COLOR.b, Config::FOOD_COLOR.a);
    SDL_Rect rect = {position.x, position.y, size, size};
    SDL_RenderFillRect(renderer, &rect);
}

SDL_Point Food::getPosition() const {
    return position;
}