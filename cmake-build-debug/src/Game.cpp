#include "Game.hpp"
#include <iostream>

Game::Game(int w, int h, int size)
    : snake(w / 2, h / 2, size, 5), food(size) {
    food.generate(w, h, snake.getBody());
}

void Game::handleInput(const SDL_Event& event) {
    Config::Controls control = Config::handleInput(event);
    if (control != Config::Controls::NONE) {
        snake.changeDirection(static_cast<Snake::Direction>(control));
    }
}

void Game::update() {
    snake.move();
}

void Game::render(Renderer& renderer) const {
    renderer.clear();
    food.draw(renderer.getRenderer());
    snake.draw(renderer.getRenderer());
    renderer.present();
}
