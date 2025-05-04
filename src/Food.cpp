#include "Food.hpp"
#include "Config.hpp"
#include <iostream>
#include <algorithm>
#include <vector>

namespace SnakeGame {

    Food::Food(int size) : position{-size, -size}, cellSize(size), rng(std::random_device{}()) {}

    void Food::generate(int screenWidth, int screenHeight, const std::deque<SDL_Point>& snakeBody, const std::vector<SDL_Point>& obstacles) {
        int maxGridX = std::max(0, (screenWidth / cellSize) - 1);
        int maxGridY = std::max(0, (screenHeight / cellSize) - 1);
        int gridArea = (maxGridX + 1) * (maxGridY + 1);
        size_t occupiedSpots = snakeBody.size() + obstacles.size();

        if (gridArea <= 0) {
            position = {-cellSize, -cellSize};
            return;
        }

        if (occupiedSpots >= static_cast<size_t>(gridArea)) {
            position = {-cellSize, -cellSize};
            std::cerr << "Warning: Cannot generate food, grid is full!" << std::endl;
            return;
        }

        std::uniform_int_distribution<int> distX(0, maxGridX);
        std::uniform_int_distribution<int> distY(0, maxGridY);

        SDL_Point potentialPos;
        bool validPosition;
        const int maxAttempts = gridArea * 3 + 50;
        int attempts = 0;

        do {
            potentialPos.x = distX(rng) * cellSize;
            potentialPos.y = distY(rng) * cellSize;
            validPosition = true;

            for (const auto& segment : snakeBody) {
                if (potentialPos.x == segment.x && potentialPos.y == segment.y) {
                    validPosition = false;
                    break;
                }
            }

            if (validPosition) {
                for (const auto& obsPos : obstacles) {
                    if (potentialPos.x == obsPos.x && potentialPos.y == obsPos.y) {
                        validPosition = false;
                        break;
                    }
                }
            }

            attempts++;

            if (!validPosition && attempts >= maxAttempts) {
                std::cerr << "Warning: Food generation fallback triggered after " << maxAttempts << " attempts." << std::endl;
                bool foundFallback = false;
                for (int y = 0; y <= maxGridY; ++y) {
                    for (int x = 0; x <= maxGridX; ++x) {
                        potentialPos = {x * cellSize, y * cellSize};
                        bool occupied = false;
                        for (const auto& seg : snakeBody) {
                            if (seg.x == potentialPos.x && seg.y == potentialPos.y) {
                                occupied = true;
                                break;
                            }
                        }
                        if (occupied) continue;
                        for (const auto& obsPos : obstacles) {
                            if (obsPos.x == potentialPos.x && obsPos.y == potentialPos.y) {
                                occupied = true;
                                break;
                            }
                        }
                        if (occupied) continue;

                        position = potentialPos;
                        foundFallback = true;
                        goto food_placed_generate;
                    }
                }

                if (!foundFallback) {
                    position = {-cellSize, -cellSize};
                    std::cerr << "Error: Could not place food even with fallback scan. Grid seems completely full." << std::endl;
                }

                food_placed_generate:;
                return;
            }

        } while (!validPosition);

        position = potentialPos;
    }

    SDL_Point Food::getPosition() const {
        return position;
    }

    void Food::forcePosition(int x, int y) {
        position.x = x;
        position.y = y;
    }

}
