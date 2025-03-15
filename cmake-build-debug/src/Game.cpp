#include "Game.hpp"
#include <iostream>

Game::Game(int w, int h, int size)
    : snake(w / 2, h / 2, size, 5), food(size),screenWidth(w), screenHeight(h), gameOver(false), score(0) {
    food.generate(w, h, snake.getBody());
}

void Game::handleInput(const SDL_Event& event) {
    Config::Controls control = Config::handleInput(event);
    if (gameOver && control == Config::Controls::RESTART) {
        reset();
    }
    else if (control != Config::Controls::NONE) {
        snake.changeDirection(static_cast<Snake::Direction>(control));
    }
}

void Game::update() {
    if (gameOver) return;
    snake.move();

    if (snake.checkSelfCollision() || snake.checkWallCollision(screenWidth, screenHeight)) {
        if (score > highScore) {
            highScore = score;
        }
        gameOver = true;
    }

    if (snake.getHeadPosition().x == food.getPosition().x &&
        snake.getHeadPosition().y == food.getPosition().y) {
        snake.grow();
        food.generate(screenWidth, screenHeight, snake.getBody());
        score++;
    }
}

void Game::render(Renderer& renderer) const {
    renderer.clear();
    food.draw(renderer.getRenderer());
    snake.draw(renderer.getRenderer());

    SDL_Color textColor = {255, 255, 255, 255};
    renderer.renderText("Score: " + std::to_string(score), 10, 10, textColor);
    renderer.renderText("High Score: " + std::to_string(highScore), 10, 40, textColor);

    if (gameOver) {
        renderer.renderText("Game Over! Press SPACE to restart", screenWidth / 4, screenHeight / 2, textColor);
    }

    renderer.present();
}

bool Game::isGameOver() const {
    return gameOver;
}

void Game::reset() {
    snake = Snake(screenWidth / 2, screenHeight / 2, 20, 5);
    food.generate(screenWidth, screenHeight, snake.getBody());
    score = 0;
    gameOver = false;
}

