#include "Game.hpp"
#include <iostream>

Game::Game(int w, int h, int size)
    : snake(w / 2, h / 2, size, 5), food(size),screenWidth(w), screenHeight(h), gameOver(false) {
    food.generate(w, h, snake.getBody());
}

void Game::handleInput(const SDL_Event& event) {
    Config::Controls control = Config::handleInput(event);
    if (control != Config::Controls::NONE) {
        snake.changeDirection(static_cast<Snake::Direction>(control));
    }
}

void Game::update() {
    if (gameOver) return;
    snake.move();
    if (snake.checkSelfCollision() || snake.checkWallCollision(screenWidth, screenHeight)) gameOver = true;
}

void Game::render(Renderer& renderer) const {
    renderer.clear();
    food.draw(renderer.getRenderer());
    snake.draw(renderer.getRenderer());
    renderer.present();
}

bool Game::isGameOver() const {
    return gameOver;
}

