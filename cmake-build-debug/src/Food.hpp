#ifndef FOOD_HPP
#define FOOD_HPP

#include <SDL.h>
#include <vector>
#include <random>

class Food {
public:
    Food(int size);
    void generate(int screenWidth, int screenHeight, const std::vector<SDL_Point>& snakeBody);
    void draw(SDL_Renderer* renderer) const;
    SDL_Point getPosition() const;

private:
    SDL_Point position;
    int size;
    std::mt19937 rng;
};

#endif
