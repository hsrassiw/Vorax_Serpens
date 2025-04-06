#ifndef FOOD_HPP
#define FOOD_HPP

#include <SDL.h>
#include <vector>
#include <random>

class Food {
public:
    explicit Food(int size);
    void generate(int screenWidth, int screenHeight, const std::vector<SDL_Point>& snakeBody);
    void draw(SDL_Renderer* renderer) const;
    [[nodiscard]] SDL_Point getPosition() const;

private:
    SDL_Point position;
    int size;
    std::mt19937 rng;
};

#endif