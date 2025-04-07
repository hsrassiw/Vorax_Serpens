#include "Game.hpp"
#include "Config.hpp"
#include "Renderer.hpp"
#include <string>
#include <iostream>
#include <algorithm>

Game::Game(int w, int h, int size)
        : screenWidth(w),
          screenHeight(h),
          cellSize(size),
          snake(calculateStartPosition().x, calculateStartPosition().y, size, Config::DEFAULT_SNAKE_LENGTH),
          food(size),
          gameOver(false),
          score(0),
          highScore(0),
          moveInterval(Config::INITIAL_SNAKE_SPEED_DELAY_MS)
{
    food.generate(screenWidth, screenHeight, snake.getBody());
    std::cout << "Game Initialized. Initial Move Interval: " << moveInterval << "ms" << std::endl;
}

SDL_Point Game::calculateStartPosition() const {
    int startGridX = (screenWidth / cellSize) / 2;
    int startGridY = (screenHeight / cellSize) / 2;
    return {startGridX * cellSize, startGridY * cellSize};
}

void Game::handleInput(const SDL_Event& event) {
    Config::Controls control = Config::handleInput(event);

    if (control == Config::Controls::RESTART && gameOver) {
        reset();
    } else if (!gameOver && control != Config::Controls::NONE && control != Config::Controls::RESTART) {
        Snake::Direction newDir;
        bool directionChanged = true;
        switch (control) {
            case Config::Controls::UP:    newDir = Snake::Direction::UP; break;
            case Config::Controls::DOWN:  newDir = Snake::Direction::DOWN; break;
            case Config::Controls::LEFT:  newDir = Snake::Direction::LEFT; break;
            case Config::Controls::RIGHT: newDir = Snake::Direction::RIGHT; break;
            default:
                directionChanged = false;
                break;
        }
        if (directionChanged) {
            snake.changeDirection(newDir);
        }
    }
}

void Game::update() {
    if (gameOver) {
        return;
    }

    snake.move();

    if (snake.checkWallCollision(screenWidth, screenHeight)) {
        gameOver = true;
    } else if (snake.checkSelfCollision()) {
        gameOver = true;
    }

    if (gameOver) {
        if (score > highScore) {
            highScore = score;
            std::cout << "New High Score: " << highScore << std::endl;
        }
        return;
    }

    if (snake.checkFoodCollision(food.getPosition())) {
        score++;
        snake.grow();
        food.generate(screenWidth, screenHeight, snake.getBody());
        increaseSpeed();
        std::cout << "Score: " << score << ". New Move Interval: " << moveInterval << "ms" << std::endl;
    }
}

void Game::render(Renderer& renderer) const {
    renderer.clear();
    food.draw(renderer.getSDLRenderer());
    snake.draw(renderer.getSDLRenderer());
    renderer.renderText("Score: " + std::to_string(score), 10, 10, Config::TEXT_COLOR);
    renderer.renderText("High Score: " + std::to_string(highScore), 10, 10 + Config::FONT_SIZE + 5, Config::TEXT_COLOR);

    if (gameOver) {
        std::string gameOverText = "Game Over! Press SPACE to restart";
        SDL_Point textSize = renderer.getTextSize(gameOverText);
        int x = (screenWidth - textSize.x) / 2;
        int y = (screenHeight - textSize.y) / 2;
        renderer.renderText(gameOverText, x, y, Config::GAMEOVER_TEXT_COLOR);
    }
    renderer.present();
}

bool Game::isGameOver() const {
    return gameOver;
}

void Game::increaseSpeed() {
    moveInterval = std::max((int)Config::MIN_MOVE_INTERVAL_MS, (int)moveInterval - (int)Config::SPEED_INCREMENT_MS);
}

Uint32 Game::getMoveInterval() const {
    return moveInterval;
}

void Game::reset() {
    std::cout << "Resetting game..." << std::endl;
    SDL_Point startPos = calculateStartPosition();
    snake = Snake(startPos.x, startPos.y, cellSize, Config::DEFAULT_SNAKE_LENGTH);
    food.generate(screenWidth, screenHeight, snake.getBody());
    score = 0;
    gameOver = false;
    moveInterval = Config::INITIAL_SNAKE_SPEED_DELAY_MS;
    std::cout << "Game Reset Complete. Move Interval: " << moveInterval << "ms" << std::endl;
}