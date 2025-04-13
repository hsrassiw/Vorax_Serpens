#include "Game.hpp"
#include "Renderer.hpp"
#include "Config.hpp"
#include <SDL_mixer.h>
#include <string>
#include <iostream>
#include <algorithm>
#include <fstream>
#include <SDL.h>

namespace SnakeGame {

    Game::Game(int w, int h, int size, Renderer& renderer)
            : screenWidth(w),
              screenHeight(h),
              cellSize(size),
              snake(calculateStartPosition().x, calculateStartPosition().y, size, Config::DEFAULT_SNAKE_LENGTH),
              food(size),
              currentState(GameState::MainMenu),
              score(0),
              highScore(loadHighScore()),
              moveInterval(Config::INITIAL_SNAKE_SPEED_DELAY_MS),
              quitRequested(false),
              timeAccumulator(0.0f),
              menuTexture(nullptr),
              backgroundTexture(nullptr),
              selectedButtonIndex(0)
    {
        initAssets(renderer);
        std::cout << "Game Initialized. Assets loaded. High Score: " << highScore << std::endl;
    }

    void Game::initAssets(Renderer& renderer) {
        backgroundTexture.reset(renderer.loadTexture(Config::BACKGROUND_IMAGE_PATH));
        if (!backgroundTexture) {
            std::cerr << "Warning: Could not load background texture from " << Config::BACKGROUND_IMAGE_PATH << ". Using default background color." << std::endl;
        } else {
            std::cout << "Background texture loaded successfully." << std::endl;
        }

        menuTexture.reset(renderer.loadTexture(Config::MENU_IMAGE_PATH));
        if (!menuTexture) {
            std::cerr << "FATAL ERROR: Could not load main menu texture from " << Config::MENU_IMAGE_PATH << "!" << std::endl;
            quitRequested = true;
            return;
        } else {
            std::cout << "Main menu texture loaded successfully." << std::endl;
        }

        foodTexture.reset(renderer.loadTexture(Config::FOOD_IMAGE_PATH));
        if (!foodTexture) {
            std::cerr << "Warning: Could not load food texture from " << Config::FOOD_IMAGE_PATH << ". Food will not be visible." << std::endl;
        } else {
            std::cout << "Food texture loaded successfully." << std::endl;
        }

        eatSound.reset(Mix_LoadWAV(Config::EAT_SOUND_PATH.c_str()));
        if (!eatSound) {
            // In ra cảnh báo nếu không nạp được, nhưng game vẫn có thể chạy
            std::cerr << "Warning: Could not load eat sound effect! Mix_Error: " << Mix_GetError() << std::endl;
        } else {
            std::cout << "Eat sound effect loaded successfully." << std::endl;
        }

        int logoHeightOnScreen = 200;
        logoDestRect = {0, 50, screenWidth, logoHeightOnScreen};

        int buttonW_Start = 485; // Adjusted width based on measurement consistency
        int buttonH_Start = 142;
        int buttonW_Options = 485;
        int buttonH_Options = 142;
        int buttonW_Quit = 485;
        int buttonH_Quit = 142;

        int buttonX_Start = 269;
        int buttonX_Options = 269;
        int buttonX_Quit = 269;

        int startY = 309;
        int optionsY = 486;
        int quitY = 659;

        menuButtons.clear();
        menuButtons.push_back({ {buttonX_Start, startY, buttonW_Start, buttonH_Start}, {}, GameState::Playing, false });
        menuButtons.push_back({ {buttonX_Options, optionsY, buttonW_Options, buttonH_Options}, {}, GameState::Options, false });
        menuButtons.push_back({ {buttonX_Quit, quitY, buttonW_Quit, buttonH_Quit}, {}, GameState::MainMenu, true });

        selectedButtonIndex = 0;


    }


    SDL_Point Game::calculateStartPosition() const {
        int startGridX = std::max(0, (screenWidth / cellSize) / 2);
        int startGridY = std::max(0, (screenHeight / cellSize) / 2);
        return {startGridX * cellSize, startGridY * cellSize};
    }

    void Game::handleInput(const SDL_Event& event) {
        Config::ControlInput control = Config::handleRawInput(event);

        if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE) {
            if (currentState == GameState::Options || currentState == GameState::Paused) {
                currentState = GameState::MainMenu;
                selectedButtonIndex = 0;
                std::cout << "Returning to Main Menu via Escape." << std::endl;
            } else {
                quitRequested = true;
                std::cout << "Quit requested via Escape key." << std::endl;
            }
            return;
        }

        switch (currentState) {
            case GameState::MainMenu:
                if (event.type == SDL_MOUSEMOTION) {
                    int mouseX = event.motion.x;
                    int mouseY = event.motion.y;
                    for (size_t i = 0; i < menuButtons.size(); ++i) {
                        SDL_Point mousePoint = {mouseX, mouseY};
                        if (SDL_PointInRect(&mousePoint, &menuButtons[i].screenRect)) {
                            if (static_cast<int>(i) != selectedButtonIndex) {
                                selectedButtonIndex = static_cast<int>(i);
                            }
                            return;
                        }
                    }
                }
                else if (event.type == SDL_MOUSEBUTTONDOWN) {
                    if (event.button.button == SDL_BUTTON_LEFT) {
                        int mouseX = event.button.x;
                        int mouseY = event.button.y;
                        SDL_Point clickPoint = {mouseX, mouseY};
                        for (size_t i = 0; i < menuButtons.size(); ++i) {
                            if (SDL_PointInRect(&clickPoint, &menuButtons[i].screenRect)) {
                                const auto& button = menuButtons[i];
                                selectedButtonIndex = static_cast<int>(i);
                                if (button.requestsQuit) {
                                    quitRequested = true;
                                    std::cout << "Quit clicked." << std::endl;
                                } else {
                                    currentState = button.targetState;
                                    if (currentState == GameState::Playing) {
                                        std::cout << "Start clicked." << std::endl;
                                        reset();
                                    } else if (currentState == GameState::Options) {
                                        std::cout << "Options clicked." << std::endl;
                                    }
                                }
                                return;
                            }
                        }
                    }
                }
                else if (event.type == SDL_KEYDOWN) {
                    if (control == Config::ControlInput::UP) {
                        selectedButtonIndex = (selectedButtonIndex - 1 + menuButtons.size()) % menuButtons.size();
                    } else if (control == Config::ControlInput::DOWN) {
                        selectedButtonIndex = (selectedButtonIndex + 1) % menuButtons.size();
                    } else if (control == Config::ControlInput::CONFIRM || control == Config::ControlInput::RESTART) {
                        if (selectedButtonIndex >= 0 && selectedButtonIndex < menuButtons.size()) {
                            const auto& button = menuButtons[selectedButtonIndex];
                            if (button.requestsQuit) {
                                quitRequested = true;
                            } else {
                                currentState = button.targetState;
                                if (currentState == GameState::Playing) {
                                    std::cout << "Starting game (Keyboard)..." << std::endl;
                                    reset();
                                } else if (currentState == GameState::Options) {
                                    std::cout << "Entering Options (Keyboard)..." << std::endl;
                                }
                            }
                        }
                    }
                }
                break;

            case GameState::Options:
                break;
            case GameState::Playing:
                if (control == Config::ControlInput::PAUSE) {
                    currentState = GameState::Paused; timeAccumulator = 0.0f; std::cout << "Game Paused" << std::endl;
                } else {
                    bool directionInput = false; Direction newDir = snake.getCurrentDirection();
                    switch (control) {
                        case Config::ControlInput::UP:    newDir = Config::Direction::UP; directionInput = true; break;
                        case Config::ControlInput::DOWN:  newDir = Config::Direction::DOWN; directionInput = true; break;
                        case Config::ControlInput::LEFT:  newDir = Config::Direction::LEFT; directionInput = true; break;
                        case Config::ControlInput::RIGHT: newDir = Config::Direction::RIGHT; directionInput = true; break;
                        default: break;
                    }
                    if (directionInput) { snake.changeDirection(newDir); }
                }
                break;
            case GameState::Paused:
                if (control == Config::ControlInput::PAUSE) {
                    currentState = GameState::Playing; timeAccumulator = 0.0f; std::cout << "Game Resumed" << std::endl;
                }
                break;
            case GameState::GameOver:
                if (control == Config::ControlInput::RESTART) {
                    std::cout << "Restarting game..." << std::endl;
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
            if(currentState != GameState::Playing) { timeAccumulator = 0.0f; break; }
            timeAccumulator -= timeStep;
        }
    }

    void Game::update() {
        snake.move();
        if (snake.checkWallCollision(screenWidth, screenHeight) || snake.checkSelfCollision()) {
            std::cout << "Collision! Game Over." << std::endl;
            currentState = GameState::GameOver;
            if (score > highScore) { highScore = score; saveHighScore(); std::cout << "New High Score: " << highScore << " (Saved)\n"; }
            return;
        }
        if (snake.checkFoodCollision(food.getPosition())) {
            if (eatSound) {
                // Tham số: kênh (-1 = kênh trống đầu tiên), chunk âm thanh, số lần lặp (0 = 1 lần)
                Mix_PlayChannel(-1, eatSound.get(), 0);
            }
            score++; snake.grow(); placeFood(); increaseSpeed();
        }
    }

    void Game::render(Renderer& renderer) const {
        renderer.clear();
        switch (currentState) {
            case GameState::MainMenu:
                if (menuTexture) {
                    SDL_Rect destRect = {0, 0, screenWidth, screenHeight};
                    renderer.drawTexture(menuTexture.get(), &destRect);
                    if (!menuButtons.empty() && selectedButtonIndex >= 0 && selectedButtonIndex < menuButtons.size()) {
                        SDL_Rect highlightRect = menuButtons[selectedButtonIndex].screenRect;
                        highlightRect.x -= 3; highlightRect.y -= 3;
                        highlightRect.w += 6; highlightRect.h += 6;
                        renderer.drawRect(&highlightRect, Config::MENU_HIGHLIGHT_COLOR, false);
                    }
                } else {
                    renderer.renderCenteredText("Error loading menu!", screenWidth, screenHeight, {255, 0, 0, 255});
                }
                break;
            case GameState::Options:
                if (backgroundTexture) {
                    SDL_Rect destRect = {0, 0, screenWidth, screenHeight};
                    renderer.drawTexture(backgroundTexture.get(), &destRect);
                }
                renderer.renderCenteredText("OPTIONS", screenWidth, screenHeight / 2 - 50, Config::TEXT_COLOR);
                renderer.renderCenteredText("(Not Implemented Yet)", screenWidth, screenHeight / 2, Config::TEXT_COLOR);
                renderer.renderCenteredText("Press ESC to Return", screenWidth, screenHeight / 2 + 50, Config::TEXT_COLOR);
                break;
            case GameState::Playing:
            case GameState::Paused:
            case GameState::GameOver:
                if (backgroundTexture) {
                    SDL_Rect destRect = {0, 0, screenWidth, screenHeight};
                    renderer.drawTexture(backgroundTexture.get(), &destRect);
                }
                if (foodTexture) {
                    SDL_Point foodPos = food.getPosition();
                    // Đảm bảo quả táo được vẽ đúng kích thước ô lưới
                    SDL_Rect foodRect = {foodPos.x, foodPos.y, cellSize, cellSize};
                    renderer.drawTexture(foodTexture.get(), &foodRect);
                }

                snake.draw(renderer.getSDLRenderer());
                renderer.renderUI(score, highScore, 10, 10, 10, 10 + Config::FONT_SIZE + 5, Config::TEXT_COLOR);
                if (currentState == GameState::Paused) {
                    renderer.renderCenteredText("Paused", screenWidth, screenHeight / 2, Config::PAUSE_TEXT_COLOR);
                    renderer.renderCenteredText("(Press ESC to Quit)", screenWidth, screenHeight/2 + Config::FONT_SIZE*1.5, Config::PAUSE_TEXT_COLOR);
                } else if (currentState == GameState::GameOver) {
                    renderer.renderCenteredText("Game Over!", screenWidth, screenHeight / 2 - Config::FONT_SIZE, Config::GAMEOVER_TEXT_COLOR);
                    renderer.renderCenteredText("Press SPACE to Restart", screenWidth, screenHeight / 2 + Config::FONT_SIZE, Config::GAMEOVER_TEXT_COLOR);
                    renderer.renderCenteredText("(Press ESC to Quit)", screenWidth, screenHeight/2 + Config::FONT_SIZE*2.5, Config::GAMEOVER_TEXT_COLOR);
                }
                break;
        }
        renderer.present();
    }

    GameState Game::getCurrentState() const {
        return currentState;
    }

    bool Game::didQuit() const {
        return quitRequested;
    }

    void Game::increaseSpeed() {
        moveInterval = std::max(static_cast<Uint32>(Config::MIN_MOVE_INTERVAL_MS),
                                moveInterval - static_cast<Uint32>(Config::SPEED_INCREMENT_MS));
    }

    void Game::placeFood() {
        food.generate(screenWidth, screenHeight, snake.getBody());
    }

    void Game::reset() {
        std::cout << "Resetting game state for playing..." << std::endl;
        SDL_Point startPos = calculateStartPosition();
        snake = Snake(startPos.x, startPos.y, cellSize, Config::DEFAULT_SNAKE_LENGTH);
        placeFood();
        score = 0;
        currentState = GameState::Playing;
        moveInterval = Config::INITIAL_SNAKE_SPEED_DELAY_MS;
        timeAccumulator = 0.0f;
    }

    int Game::loadHighScore() {
        std::ifstream file(Config::HIGHSCORE_FILE);
        int loadedScore = 0;
        if (file.is_open()) {
            if (!(file >> loadedScore) || file.fail() || loadedScore < 0) {
                if(loadedScore < 0) std::cerr << "Warning: Invalid negative high score found in " << Config::HIGHSCORE_FILE << ". Resetting to 0." << std::endl;
                else std::cerr << "Warning: Could not read valid high score from " << Config::HIGHSCORE_FILE << ". Resetting to 0." << std::endl;
                loadedScore = 0;
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