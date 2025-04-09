#include "Food.hpp"
#include "Config.hpp"
#include <iostream>
#include <algorithm>

namespace SnakeGame {

    Food::Food(int size) : position{0, 0}, cellSize(size), rng(std::random_device{}()) {}

    void Food::generate(int screenWidth, int screenHeight, const std::deque<SDL_Point>& snakeBody) {
        int maxGridX = std::max(0, (screenWidth / cellSize) - 1);
        int maxGridY = std::max(0, (screenHeight / cellSize) - 1);
        int gridArea = (maxGridX + 1) * (maxGridY + 1);

        if (gridArea > 0 && snakeBody.size() >= static_cast<size_t>(gridArea)) {
            std::cerr << "Warning: No valid space left to generate food! Snake fills the grid." << std::endl;
            position = {-cellSize, -cellSize};
            return;
        }

        if (gridArea <= 0) {
            std::cerr << "Warning: Grid area is zero or negative. Cannot generate food." << std::endl;
            position = {-cellSize, -cellSize};
            return;
        }

        std::uniform_int_distribution<int> distX(0, maxGridX);
        std::uniform_int_distribution<int> distY(0, maxGridY);

        bool validPosition;
        int potentialX, potentialY;
        const int maxAttempts = gridArea * 2 + 10;
        int attempts = 0;

        do {
            potentialX = distX(rng) * cellSize;
            potentialY = distY(rng) * cellSize;
            validPosition = true;

            for (const auto& segment : snakeBody) {
                if (potentialX == segment.x && potentialY == segment.y) {
                    validPosition = false;
                    break;
                }
            }

            attempts++;
            if (!validPosition && attempts >= maxAttempts) {
                std::cerr << "Warning: Could not find a valid food position after "
                          << maxAttempts << " attempts. Snake length: "
                          << snakeBody.size() << "/" << gridArea << "." << std::endl;

                bool originOccupied = false;
                for(const auto& segment : snakeBody) {
                    if(segment.x == 0 && segment.y == 0) {
                        originOccupied = true;
                        break;
                    }
                }
                if (originOccupied) {
                    position = {-cellSize, -cellSize};
                    std::cerr << "Fallback position (0,0) is also occupied. Placing food off-screen." << std::endl;
                } else {
                    position = {0, 0};
                }
                return;
            }

        } while (!validPosition);

        position.x = potentialX;
        position.y = potentialY;
    }

    void Food::draw(SDL_Renderer* renderer) const {
        if (!renderer) return;
        SDL_SetRenderDrawColor(renderer, Config::FOOD_COLOR.r, Config::FOOD_COLOR.g, Config::FOOD_COLOR.b, Config::FOOD_COLOR.a);
        SDL_Rect rect = {position.x, position.y, cellSize, cellSize};
        SDL_RenderFillRect(renderer, &rect);
    }

    SDL_Point Food::getPosition() const {
        return position;
    }

}