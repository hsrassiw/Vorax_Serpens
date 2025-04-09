#ifndef FOOD_HPP
#define FOOD_HPP

#include <SDL.h>
#include <vector>
#include <random>
#include <deque>

namespace SnakeGame {

    class Food {
    public:
        explicit Food(int cellSize);
        void generate(int screenWidth, int screenHeight, const std::deque<SDL_Point>& snakeBody);
        void draw(SDL_Renderer* renderer) const;
        [[nodiscard]] SDL_Point getPosition() const;

    private:
        SDL_Point position;
        int cellSize;
        std::mt19937 rng;
    };

}

#endif