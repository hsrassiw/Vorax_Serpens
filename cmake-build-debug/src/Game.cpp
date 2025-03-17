#include "Game.hpp"
#include "Renderer.hpp"
#include "Snake.hpp"
#include <SDL_ttf.h>
#include "Config.hpp"


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

    if (snake.checkCollision(food.getPosition())) {
        snake.grow();
        food.generate(screenWidth, screenHeight, snake.getBody());
        score++;
    }

}

void Game::render(const Renderer& renderer) const {
    renderer.clear();
    food.draw(renderer.getRenderer());
    snake.draw(renderer.getRenderer());

    renderer.renderText("Score: " + std::to_string(score), 10, 10, Config :: TEXT_COLOR);
    renderer.renderText("High Score: " + std::to_string(highScore), 10, 40, Config :: TEXT_COLOR);

    if (gameOver) {
        int textWidth, textHeight;


        TTF_SizeText(renderer.getFont(), "Game Over! Press SPACE to restart", &textWidth, &textHeight);


        int x = (screenWidth - textWidth) / 2;
        int y = (screenHeight - textHeight) / 2;

        renderer.renderText("Game Over! Press SPACE to restart", x, y, Config::TEXT_COLOR);
    }


    renderer.present();
}

bool Game::isGameOver() const {
    return gameOver;
}

void Game::reset() {
    int newSize = snake.getSize();


    int startX = (screenWidth / 2) - ((screenWidth / 2) % newSize);
    int startY = (screenHeight / 2) - ((screenHeight / 2) % newSize);

    snake = Snake(startX, startY, newSize, 5);
    food.setSize(newSize);
    food.generate(screenWidth, screenHeight, snake.getBody());

    score = 0;
    gameOver = false;
}


