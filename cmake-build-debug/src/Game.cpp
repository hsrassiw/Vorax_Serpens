#include "Game.hpp"
#include "Renderer.hpp"
#include <string>
#include <iostream>
#include <algorithm>
#include <fstream>
#include <limits>

namespace SnakeGame {

    Game::Game(int w, int h, int size)
            : screenWidth(w),
              screenHeight(h),
              cellSize(size),
              snake(calculateStartPosition().x, calculateStartPosition().y, size, Config::DEFAULT_SNAKE_LENGTH),
              food(size),
              currentState(GameState::Playing),
              score(0),
              highScore(loadHighScore()),
              moveInterval(Config::INITIAL_SNAKE_SPEED_DELAY_MS),
              timeAccumulator(0.0f)
    {
        placeFood();
        std::cout << "Game Initialized. High Score: " << highScore << ". Initial Move Interval: " << moveInterval << "ms" << std::endl;
    }

    SDL_Point Game::calculateStartPosition() const {
        int startGridX = std::max(0, (screenWidth / cellSize) / 2);
        int startGridY = std::max(0, (screenHeight / cellSize) / 2);
        return {startGridX * cellSize, startGridY * cellSize};
    }

    void Game::handleInput(const SDL_Event& event) {
        Config::ControlInput control = Config::handleRawInput(event);

        switch (currentState) {
            case GameState::Playing:
                if (control == Config::ControlInput::PAUSE) {
                    currentState = GameState::Paused;
                    timeAccumulator = 0.0f;
                    std::cout << "Game Paused" << std::endl;
                } else {
                    bool directionInput = false;
                    Direction newDir = snake.getCurrentDirection();

                    switch (control) {
                        case Config::ControlInput::UP:    newDir = Direction::UP; directionInput = true; break;
                        case Config::ControlInput::DOWN:  newDir = Direction::DOWN; directionInput = true; break;
                        case Config::ControlInput::LEFT:  newDir = Direction::LEFT; directionInput = true; break;
                        case Config::ControlInput::RIGHT: newDir = Direction::RIGHT; directionInput = true; break;
                        default: break;
                    }
                    if (directionInput) {
                        snake.changeDirection(newDir);
                    }
                }
                break;

            case GameState::Paused:
                if (control == Config::ControlInput::PAUSE) {
                    currentState = GameState::Playing;
                    timeAccumulator = 0.0f;
                    std::cout << "Game Resumed" << std::endl;
                }
                break;

            case GameState::GameOver:
                if (control == Config::ControlInput::RESTART) {
                    reset();
                }
                break;
        }
    }

    void Game::runFrame(float deltaTime) {
        if (currentState != GameState::Playing) {
            timeAccumulator = 0.0f;
            return;
        }

        timeAccumulator += deltaTime;
        const float timeStep = static_cast<float>(moveInterval) / 1000.0f;

        while (timeAccumulator >= timeStep && timeStep > 0) {
            update();
            if(currentState != GameState::Playing) {
                timeAccumulator = 0.0f;
                break;
            }
            timeAccumulator -= timeStep;
        }
    }


    void Game::update() {

        snake.move();

        if (snake.checkWallCollision(screenWidth, screenHeight) || snake.checkSelfCollision()) {
            currentState = GameState::GameOver;
            if (score > highScore) {
                highScore = score;
                saveHighScore();
                std::cout << "New High Score: " << highScore << " (Saved)" << std::endl;
            }
            return;
        }

        if (snake.checkFoodCollision(food.getPosition())) {
            score++;
            snake.grow();
            placeFood();
            increaseSpeed();
        }
    }

    void Game::render(Renderer& renderer) const {
        renderer.clear();

        food.draw(renderer.getSDLRenderer());
        snake.draw(renderer.getSDLRenderer());

        renderer.renderUI(score, highScore, 10, 10, 10, 10 + Config::FONT_SIZE + 5, Config::TEXT_COLOR);

        switch (currentState) {
            case GameState::GameOver:
                renderer.renderCenteredText("Game Over! Press SPACE to restart", screenWidth, screenHeight, Config::GAMEOVER_TEXT_COLOR);
                break;
            case GameState::Paused:
                renderer.renderCenteredText("Paused", screenWidth, screenHeight, Config::PAUSE_TEXT_COLOR);
                break;
            case GameState::Playing:

                break;
        }

        renderer.present();
    }

    bool Game::isGameOver() const {
        return currentState == GameState::GameOver;
    }

    bool Game::isPaused() const {
        return currentState == GameState::Paused;
    }

    GameState Game::getCurrentState() const {
        return currentState;
    }

    void Game::increaseSpeed() {
        moveInterval = std::max(static_cast<Uint32>(Config::MIN_MOVE_INTERVAL_MS),
                                moveInterval - static_cast<Uint32>(Config::SPEED_INCREMENT_MS));
    }

    void Game::placeFood() {
        food.generate(screenWidth, screenHeight, snake.getBody());
    }


    void Game::reset() {
        std::cout << "Resetting game..." << std::endl;
        SDL_Point startPos = calculateStartPosition();
        snake = Snake(startPos.x, startPos.y, cellSize, Config::DEFAULT_SNAKE_LENGTH);
        placeFood();
        score = 0;
        currentState = GameState::Playing;
        moveInterval = Config::INITIAL_SNAKE_SPEED_DELAY_MS;
        timeAccumulator = 0.0f;

        std::cout << "Game Reset Complete. Move Interval: " << moveInterval << "ms" << std::endl;
    }

    int Game::loadHighScore() {
        std::ifstream file(Config::HIGHSCORE_FILE);
        int loadedScore = 0;

        if (file.is_open()) {
            if (!(file >> loadedScore) || file.fail()) {
                loadedScore = 0;
                std::cerr << "Warning: Could not read valid high score from " << Config::HIGHSCORE_FILE << ". Resetting to 0." << std::endl;
            } else if (loadedScore < 0) {
                loadedScore = 0;
                std::cerr << "Warning: Invalid negative high score found in " << Config::HIGHSCORE_FILE << ". Resetting to 0." << std::endl;
            }
            file.close();
        } else {
            std::cout << "High score file (" << Config::HIGHSCORE_FILE << ") not found or could not be opened. Starting with high score 0." << std::endl;
            loadedScore = 0;
        }
        return loadedScore;
    }

    void Game::saveHighScore() const {
        std::ofstream file(Config::HIGHSCORE_FILE, std::ios::trunc);

        if (file.is_open()) {
            file << highScore;
            if (file.fail()) {
                std::cerr << "Error: Failed to write high score to " << Config::HIGHSCORE_FILE << std::endl;
            }
            file.close();
        } else {
            std::cerr << "Error: Could not open " << Config::HIGHSCORE_FILE << " for writing high score." << std::endl;
        }
    }

}