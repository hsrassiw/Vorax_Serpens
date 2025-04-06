#include "Game.hpp"
#include "Config.hpp"
#include "Renderer.hpp"
#include <string>
#include <iostream>

Game::Game(int w, int h, int size)
        : screenWidth(w),
          screenHeight(h),
          cellSize(size),
          snake(calculateStartPosition().x, calculateStartPosition().y, size, Config::DEFAULT_SNAKE_LENGTH),
          food(size),
          gameOver(false),
          score(0),
          highScore(0)
{
    food.generate(screenWidth, screenHeight, snake.getBody());
    std::cout << "Game Initialized. Snake at (" << snake.getHeadPosition().x << ", " << snake.getHeadPosition().y << ")" << std::endl;
    std::cout << "Food at (" << food.getPosition().x << ", " << food.getPosition().y << ")" << std::endl;
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
        std::cout << "Collision: Wall" << std::endl;
        gameOver = true;
    }
    else if (snake.checkSelfCollision()) {
        std::cout << "Collision: Self" << std::endl;
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
        std::cout << "Collision: Food" << std::endl;
        score++;
        snake.grow();
        food.generate(screenWidth, screenHeight, snake.getBody());
        std::cout << "Score: " << score << ", New Food at (" << food.getPosition().x << ", " << food.getPosition().y << ")" << std::endl;
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

void Game::reset() {
    std::cout << "Resetting game..." << std::endl;
    SDL_Point startPos = calculateStartPosition();

    snake = Snake(startPos.x, startPos.y, cellSize, Config::DEFAULT_SNAKE_LENGTH);
    food.generate(screenWidth, screenHeight, snake.getBody());

    score = 0;
    gameOver = false;
    std::cout << "Game Reset Complete. Snake at (" << snake.getHeadPosition().x << ", " << snake.getHeadPosition().y << "), Food at (" << food.getPosition().x << ", " << food.getPosition().y << ")" << std::endl;
}