#include "Game.hpp"
#include "Renderer.hpp"
#include "Config.hpp"
#include <SDL_mixer.h>
#include <string>
#include <iostream>
#include <algorithm>
#include <fstream>
#include <random>
#include <sstream>
#include <vector>
#include <SDL.h>
#include <cstring>

namespace SnakeGame {

    namespace {
        void PlaySoundEffect(Mix_Chunk* chunk, bool soundEnabled) {
            if (soundEnabled && chunk) {
                Mix_PlayChannel(-1, chunk, 0);
            }
        }
    }

    Game::Game(int w, int h, int size, Renderer& renderer)
            : screenWidth(w),
              screenHeight(h),
              cellSize(size),
              snake(calculateStartPosition().x, calculateStartPosition().y, size, Config::DEFAULT_SNAKE_LENGTH),
              food(size),
              currentState(GameState::MainMenu),
              currentGameMode(GameMode::Classic),
              soundEnabled(true),
              score(0),
              moveInterval(Config::INITIAL_SNAKE_SPEED_DELAY_MS),
              quitRequested(false),
              timeAccumulator(0.0f),
              isBoosting(false),
              boostCostTimer(0.0f),
              boostCostCycles(0),
              selectedButtonIndex(0),
              selectedOptionIndex(0),
              isEnteringName(false),
              nextObstacleScoreThreshold(Config::OBSTACLE_ADD_SCORE_INTERVAL),
              rng(std::random_device{}())
    {
        loadHighScores();
        initAssets(renderer);
        initOptions();
        generateObstacles();
        placeFood();
        std::cout << "Game Initialized. Mode: "
                  << (currentGameMode == GameMode::Classic ? "Classic" : "Portal")
                  << ". High Scores Loaded. Ready for Main Menu." << std::endl;
    }

    void Game::initAssets(Renderer& renderer) {
        backgroundTexture.reset(renderer.loadTexture(Config::BACKGROUND_IMAGE_PATH));
        menuTexture.reset(renderer.loadTexture(Config::MENU_IMAGE_PATH));
        foodTexture.reset(renderer.loadTexture(Config::FOOD_IMAGE_PATH));

        if (!menuTexture) {
            throw std::runtime_error("FATAL ERROR: Could not load main menu texture: " + Config::MENU_IMAGE_PATH);
        } else { std::cout << "Main menu texture loaded." << std::endl; }

        if (!backgroundTexture) {
            std::cerr << "Warning: Failed to load background texture " << Config::BACKGROUND_IMAGE_PATH << std::endl;
        } else { std::cout << "Background texture loaded." << std::endl; }

        if (!foodTexture) {
            std::cerr << "Warning: Failed to load food texture " << Config::FOOD_IMAGE_PATH << std::endl;
        } else { std::cout << "Food texture loaded." << std::endl; }

        pausedTextTexture.reset(renderer.createTextTexture("Paused", Config::PAUSE_TEXT_COLOR));
        gameOverTextTexture.reset(renderer.createTextTexture("Game Over!", Config::GAMEOVER_TEXT_COLOR));

        if(pausedTextTexture) renderer.queryTexture(pausedTextTexture.get(), nullptr, nullptr, &pausedTextRect.w, &pausedTextRect.h);
        else std::cerr << "Warning: Failed to create Paused text texture." << std::endl;
        if(gameOverTextTexture) renderer.queryTexture(gameOverTextTexture.get(), nullptr, nullptr, &gameOverTextRect.w, &gameOverTextRect.h);
        else std::cerr << "Warning: Failed to create Game Over text texture." << std::endl;

        // Chỉ tải 3 âm thanh cần thiết
        eatSound.reset(Mix_LoadWAV(Config::EAT_SOUND_PATH.c_str()));
        collisionSound.reset(Mix_LoadWAV(Config::COLLISION_SOUND_PATH.c_str()));
        gameOverSound.reset(Mix_LoadWAV(Config::GAME_OVER_SOUND_PATH.c_str()));


        if (!eatSound) { std::cerr << "Warning: Could not load eat sound effect! Mix_Error: " << Mix_GetError() << std::endl; }
        else { std::cout << "Eat sound effect loaded." << std::endl; Mix_VolumeChunk(eatSound.get(), MIX_MAX_VOLUME / 2); }
        if (!collisionSound) std::cerr << "Warning: Could not load collision sound! Mix_Error: " << Mix_GetError() << std::endl;
        if (!gameOverSound) std::cerr << "Warning: Could not load game over sound! Mix_Error: " << Mix_GetError() << std::endl;


        const int BUTTON_W = 485;
        const int BUTTON_H = 142;
        const int BUTTON_X = 269;
        const int START_Y = 309;
        const int OPTIONS_Y = START_Y + BUTTON_H + 35;
        const int QUIT_Y = OPTIONS_Y + BUTTON_H + 35;

        menuButtons.clear();
        menuButtons.push_back({ {BUTTON_X, START_Y, BUTTON_W, BUTTON_H}, GameState::Playing, false, "Start Game" });
        menuButtons.push_back({ {BUTTON_X, OPTIONS_Y, BUTTON_W, BUTTON_H}, GameState::Options, false, "Options" });
        menuButtons.push_back({ {BUTTON_X, QUIT_Y, BUTTON_W, BUTTON_H}, GameState::MainMenu, true, "Quit" });

        selectedButtonIndex = 0;
    }

    void Game::initOptions() {
        optionsMenuItems.clear();
        optionsMenuItems.push_back({ "", OptionAction::TOGGLE_MODE });
        optionsMenuItems.push_back({ "", OptionAction::TOGGLE_SOUND });
        optionsMenuItems.push_back({ "Reset High Scores", OptionAction::RESET_HIGHSCORES });
        optionsMenuItems.push_back({ "Back", OptionAction::GOTO_MAINMENU });
        updateOptionTexts();
        selectedOptionIndex = 0;
        optionsMenuItemRects.resize(optionsMenuItems.size());
    }

    void Game::updateOptionTexts() {
        for (auto& item : optionsMenuItems) {
            if (item.action == OptionAction::TOGGLE_MODE) {
                item.text = "Game Mode: " + std::string(currentGameMode == GameMode::Classic ? "Classic" : "Portal Walls");
            } else if (item.action == OptionAction::TOGGLE_SOUND) {
                item.text = "Sound: " + std::string(soundEnabled ? "On" : "Off");
            }
        }
    }

    SDL_Point Game::calculateStartPosition() const {
        int startGridX = std::max(0, (screenWidth / cellSize) / 2);
        int startGridY = std::max(0, (screenHeight / cellSize) / 2);
        return {startGridX * cellSize, startGridY * cellSize};
    }

    void Game::handleInput(const SDL_Event& event) {
        if (event.type == SDL_QUIT) {
            quitRequested = true;
            std::cout << "Quit requested via window close button." << std::endl;
            return;
        }

        if (currentState == GameState::EnteringHighScore) {
            handleHighScoreInput(event);
            return;
        }

        if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE) {
            handleEscapeKey();
            return;
        }

        Config::ControlInput control = Config::handleRawInput(event);

        switch (currentState) {
            case GameState::MainMenu:        handleMainMenuInput(event, control); break;
            case GameState::Options:         handleOptionsInput(event, control);  break;
            case GameState::Playing:         handlePlayingInput(event, control);  break;
            case GameState::Paused:          handlePausedInput(event, control);   break;
            case GameState::GameOver:        handleGameOverInput(event, control); break;
            case GameState::EnteringHighScore: break;
        }
    }

    void Game::handleHighScoreInput(const SDL_Event& event) {
        if (event.type == SDL_KEYDOWN) {
            if (event.key.keysym.sym == SDLK_RETURN || event.key.keysym.sym == SDLK_KP_ENTER) {
                if (!currentPlayerNameInput.empty()) {
                    addHighScore(currentPlayerNameInput, score);
                    saveHighScores();
                    currentState = GameState::GameOver;
                    if(isEnteringName) SDL_StopTextInput();
                    isEnteringName = false;
                    std::cout << "High score saved for: " << currentPlayerNameInput << std::endl;
                } else {
                    std::cout << "Player name cannot be empty." << std::endl;
                }
            }
            else if (event.key.keysym.sym == SDLK_BACKSPACE && !currentPlayerNameInput.empty()) {
                currentPlayerNameInput.pop_back();
            }
            else if (event.key.keysym.sym == SDLK_ESCAPE) {
                currentState = GameState::GameOver;
                if(isEnteringName) SDL_StopTextInput();
                isEnteringName = false;
                std::cout << "High score entry cancelled. Returning to Game Over screen." << std::endl;
            }
        }
        else if (event.type == SDL_TEXTINPUT) {
            if (currentPlayerNameInput.length() < 15 && !(SDL_GetModState() & KMOD_CTRL) &&
                event.text.text[0] >= ' ' && event.text.text[0] <= '~')
            {
                currentPlayerNameInput += event.text.text;
            }
        }
    }

    void Game::handleEscapeKey() {
        std::cout << "Escape key pressed in state: " << static_cast<int>(currentState) << std::endl;
        switch (currentState) {
            case GameState::Options:
                currentState = GameState::MainMenu;
                selectedButtonIndex = 0;
                std::cout << "Returning to Main Menu from Options." << std::endl;
                break;
            case GameState::Paused:
                currentState = GameState::MainMenu;
                selectedButtonIndex = 0;
                std::cout << "Returning to Main Menu from Paused." << std::endl;
                break;
            case GameState::GameOver:
                currentState = GameState::MainMenu;
                selectedButtonIndex = 0;
                std::cout << "Returning to Main Menu from Game Over." << std::endl;
                break;
            case GameState::Playing:
                currentState = GameState::Paused;
                timeAccumulator = 0.0f;
                std::cout << "Game Paused." << std::endl;
                break;
            case GameState::EnteringHighScore:
                currentState = GameState::GameOver;
                if(isEnteringName) SDL_StopTextInput();
                isEnteringName = false;
                std::cout << "High score entry cancelled via ESC. Returning to Game Over screen." << std::endl;
                break;
            case GameState::MainMenu:
                quitRequested = true;
                std::cout << "Quit requested via Escape from Main Menu." << std::endl;
                break;
        }
    }

    void Game::handleMainMenuInput(const SDL_Event& event, Config::ControlInput control) {
        //bool selectionChanged = false; // Không cần biến này nếu không có âm thanh điều hướng
        int oldIndex = selectedButtonIndex;

        if (event.type == SDL_MOUSEMOTION) {
            int mouseX = event.motion.x;
            int mouseY = event.motion.y;
            for (size_t i = 0; i < menuButtons.size(); ++i) {
                SDL_Point mousePoint = {mouseX, mouseY};
                if (SDL_PointInRect(&mousePoint, &menuButtons[i].screenRect)) {
                    if (static_cast<int>(i) != selectedButtonIndex) {
                        selectedButtonIndex = static_cast<int>(i);
                        //selectionChanged = true;
                    }
                    break;
                }
            }
            //if (oldIndex != selectedButtonIndex) selectionChanged = true; // Bỏ comment nếu vẫn muốn cờ này vì lý do khác
        }
        else if (event.type == SDL_MOUSEBUTTONDOWN) {
            if (event.button.button == SDL_BUTTON_LEFT) {
                int mouseX = event.button.x;
                int mouseY = event.button.y;
                SDL_Point clickPoint = {mouseX, mouseY};
                for (size_t i = 0; i < menuButtons.size(); ++i) {
                    if (SDL_PointInRect(&clickPoint, &menuButtons[i].screenRect)) {
                        // PlaySoundEffect(menuConfirmSound.get(), soundEnabled); // Bỏ âm thanh confirm
                        selectedButtonIndex = static_cast<int>(i);
                        activateMainMenuButton(selectedButtonIndex);
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
            } else if (control == Config::ControlInput::CONFIRM) {
                if (selectedButtonIndex >= 0 && selectedButtonIndex < static_cast<int>(menuButtons.size())) {
                    // PlaySoundEffect(menuConfirmSound.get(), soundEnabled); // Bỏ âm thanh confirm
                    activateMainMenuButton(selectedButtonIndex);
                    return;
                }
            }
            //if(oldIndex != selectedButtonIndex) selectionChanged = true; // Bỏ comment nếu vẫn muốn cờ này vì lý do khác
        }

        //if (selectionChanged) {
        //    PlaySoundEffect(menuNavigateSound.get(), soundEnabled); // Bỏ âm thanh điều hướng
        //}
    }

    void Game::activateMainMenuButton(int index) {
        if (index < 0 || index >= static_cast<int>(menuButtons.size())) return;

        const auto& button = menuButtons[index];
        std::cout << "Main Menu Button activated: " << button.debugText << std::endl;

        if (button.requestsQuit) {
            quitRequested = true;
        } else {
            if (button.targetState == GameState::Playing) {
                std::cout << "Starting new game..." << std::endl;
                reset();
            }
            else {
                currentState = button.targetState;
                if(currentState == GameState::Options) {
                    selectedOptionIndex = 0;
                    updateOptionTexts();
                    for(auto& rect : optionsMenuItemRects) { rect = {0,0,0,0}; }
                    std::cout << "Entering Options..." << std::endl;
                }
            }
        }
    }

    void Game::handleOptionsInput(const SDL_Event& event, Config::ControlInput control) {
        //bool selectionChanged = false; // Không cần biến này nếu không có âm thanh điều hướng
        int oldIndex = selectedOptionIndex;

        if (event.type == SDL_MOUSEMOTION) {
            int mouseX = event.motion.x;
            int mouseY = event.motion.y;
            SDL_Point mousePoint = {mouseX, mouseY};
            bool foundHover = false;
            for (int i = 0; i < static_cast<int>(optionsMenuItemRects.size()); ++i) {
                if (optionsMenuItemRects[i].w > 0 && SDL_PointInRect(&mousePoint, &optionsMenuItemRects[i])) {
                    if (i != selectedOptionIndex) {
                        selectedOptionIndex = i;
                        //selectionChanged = true;
                    }
                    foundHover = true;
                    break;
                }
            }
            //if (oldIndex != selectedOptionIndex) selectionChanged = true; // Bỏ comment nếu vẫn muốn cờ này vì lý do khác
        } else if (event.type == SDL_MOUSEBUTTONDOWN) {
            if (event.button.button == SDL_BUTTON_LEFT) {
                int mouseX = event.button.x;
                int mouseY = event.button.y;
                SDL_Point clickPoint = {mouseX, mouseY};
                for (int i = 0; i < static_cast<int>(optionsMenuItemRects.size()); ++i) {
                    if (optionsMenuItemRects[i].w > 0 && SDL_PointInRect(&clickPoint, &optionsMenuItemRects[i])) {
                        if (i != selectedOptionIndex) {
                            selectedOptionIndex = i;
                            //selectionChanged = true;
                        }
                        // PlaySoundEffect(menuConfirmSound.get(), soundEnabled); // Bỏ âm thanh confirm
                        activateOptionItem(selectedOptionIndex);
                        return;
                    }
                }
            }
        } else if (event.type == SDL_KEYDOWN) {
            if (selectedOptionIndex < 0 || selectedOptionIndex >= static_cast<int>(optionsMenuItems.size())) {
                selectedOptionIndex = 0;
            }

            if (control == Config::ControlInput::UP) {
                selectedOptionIndex = (selectedOptionIndex - 1 + optionsMenuItems.size()) % optionsMenuItems.size();
            } else if (control == Config::ControlInput::DOWN) {
                selectedOptionIndex = (selectedOptionIndex + 1) % optionsMenuItems.size();
            } else if (control == Config::ControlInput::CONFIRM || control == Config::ControlInput::LEFT || control == Config::ControlInput::RIGHT) {
                if (selectedOptionIndex >= 0 && selectedOptionIndex < static_cast<int>(optionsMenuItems.size())) {
                    // PlaySoundEffect(menuConfirmSound.get(), soundEnabled); // Bỏ âm thanh confirm
                    activateOptionItem(selectedOptionIndex);
                }
            }
            //if(oldIndex != selectedOptionIndex) selectionChanged = true; // Bỏ comment nếu vẫn muốn cờ này vì lý do khác
        }

        //if (selectionChanged) {
        //    PlaySoundEffect(menuNavigateSound.get(), soundEnabled); // Bỏ âm thanh điều hướng
        //}
    }

    void Game::activateOptionItem(int index) {
        if (index < 0 || index >= static_cast<int>(optionsMenuItems.size())) return;

        OptionItem& item = optionsMenuItems[index];
        std::cout << "Options item activated: " << item.text << " (Index: " << index << ")" << std::endl;

        switch (item.action) {
            case OptionAction::TOGGLE_MODE:
                toggleGameMode();
                break;
            case OptionAction::TOGGLE_SOUND:
                soundEnabled = !soundEnabled;
                std::cout << "Sound toggled " << (soundEnabled ? "ON" : "OFF") << std::endl;
                Mix_Volume(-1, soundEnabled ? MIX_MAX_VOLUME : 0);
                if (eatSound) Mix_VolumeChunk(eatSound.get(), soundEnabled ? MIX_MAX_VOLUME / 2 : 0);
                updateOptionTexts();
                break;
            case OptionAction::RESET_HIGHSCORES:
                std::cout << "Resetting high scores." << std::endl;
                highScores.clear();
                saveHighScores();
                break;
            case OptionAction::GOTO_MAINMENU:
                currentState = GameState::MainMenu;
                selectedButtonIndex = 0;
                std::cout << "Returning to Main Menu." << std::endl;
                break;
        }
    }

    void Game::toggleGameMode() {
        if (currentGameMode == GameMode::Classic) {
            currentGameMode = GameMode::PortalWalls;
        } else {
            currentGameMode = GameMode::Classic;
        }
        std::cout << "Game mode toggled to: "
                  << (currentGameMode == GameMode::Classic ? "Classic" : "Portal Walls") << std::endl;
        if (currentState == GameState::Options) {
            updateOptionTexts();
        }
    }

    void Game::handlePlayingInput(const SDL_Event& event, Config::ControlInput control) {
        if (control == Config::ControlInput::PAUSE) {
            currentState = GameState::Paused;
            timeAccumulator = 0.0f;
            std::cout << "Game Paused" << std::endl;
        } else {
            Direction requestedDir = snake.getCurrentDirection();
            bool directionInput = false;
            switch (control) {
                case Config::ControlInput::UP:    requestedDir = Direction::UP; directionInput = true; break;
                case Config::ControlInput::DOWN:  requestedDir = Direction::DOWN; directionInput = true; break;
                case Config::ControlInput::LEFT:  requestedDir = Direction::LEFT; directionInput = true; break;
                case Config::ControlInput::RIGHT: requestedDir = Direction::RIGHT; directionInput = true; break;
                default: break;
            }
            if (directionInput) {
                snake.queueDirectionChange(requestedDir);
            }
        }
    }

    void Game::handlePausedInput(const SDL_Event& event, Config::ControlInput control) {
        if (control == Config::ControlInput::PAUSE || control == Config::ControlInput::CONFIRM) {
            currentState = GameState::Playing;
            timeAccumulator = 0.0f;
            std::cout << "Game Resumed" << std::endl;
        }
    }

    void Game::handleGameOverInput(const SDL_Event& event, Config::ControlInput control) {
        if (control == Config::ControlInput::RESTART) {
            std::cout << "Restarting game..." << std::endl;
            reset();
        }
    }

    void Game::runFrame(float deltaTime) {
        handleBoosting(deltaTime);

        if (currentState != GameState::Playing) {
            if (isBoosting) {
                isBoosting = false;
                boostCostTimer = 0.0f;
                boostCostCycles = 0;
            }
            if (currentState != GameState::EnteringHighScore) {
                timeAccumulator = 0.0f;
            }
            return;
        }

        timeAccumulator += deltaTime;
        Uint32 currentRequiredInterval = isBoosting ? Config::BOOST_MOVE_INTERVAL_MS : moveInterval;
        const float timeStep = static_cast<float>(currentRequiredInterval) / 1000.0f;

        while (timeAccumulator >= timeStep && timeStep > 0.0f) {
            update();

            if(currentState != GameState::Playing) {
                timeAccumulator = 0.0f;
                if (isBoosting) { isBoosting = false; boostCostTimer = 0.0f; boostCostCycles = 0;}
                break;
            }

            updateObstacles();

            if (checkObstacleCollision(snake.getHeadPosition())) {
                std::cout << "Collision! Game Over. Reason: Obstacle moved into snake head." << std::endl;
                PlaySoundEffect(collisionSound.get(), soundEnabled);
                PlaySoundEffect(gameOverSound.get(), soundEnabled);
                if (isHighScore(score)) { currentState = GameState::EnteringHighScore; currentPlayerNameInput = ""; if(!isEnteringName) SDL_StartTextInput(); isEnteringName = true; }
                else { currentState = GameState::GameOver; if(isEnteringName) SDL_StopTextInput(); isEnteringName = false; }
                timeAccumulator = 0.0f; if (isBoosting) { isBoosting = false; boostCostTimer = 0.0f; boostCostCycles = 0;}
                break;
            }
            const auto& snakeBody = snake.getBody();
            bool bodyCollision = false;
            for(const auto& obs : obstacles) {
                for(size_t i = 1; i < snakeBody.size(); ++i) {
                    if (obs.position.x == snakeBody[i].x && obs.position.y == snakeBody[i].y) {
                        bodyCollision = true; break;
                    }
                } if (bodyCollision) break;
            }
            if (bodyCollision) {
                std::cout << "Collision! Game Over. Reason: Obstacle moved into snake body." << std::endl;
                PlaySoundEffect(collisionSound.get(), soundEnabled);
                PlaySoundEffect(gameOverSound.get(), soundEnabled);
                if (isHighScore(score)) { currentState = GameState::EnteringHighScore; currentPlayerNameInput = ""; if(!isEnteringName) SDL_StartTextInput(); isEnteringName = true; }
                else { currentState = GameState::GameOver; if(isEnteringName) SDL_StopTextInput(); isEnteringName = false; }
                timeAccumulator = 0.0f; if (isBoosting) { isBoosting = false; boostCostTimer = 0.0f; boostCostCycles = 0;}
                break;
            }

            timeAccumulator -= timeStep;

            if (isBoosting && (score <= 0 || snake.getBody().size() <= Config::MIN_BOOST_LENGTH)) {
                // PlaySoundEffect(boostEndSound.get(), soundEnabled); // Bỏ âm thanh dừng boost
                isBoosting = false;
                boostCostTimer = 0.0f;
                boostCostCycles = 0;
                std::cout << "Boost stopped mid-step due to low score/length." << std::endl;
                currentRequiredInterval = moveInterval;
            }
        }
    }

    void Game::handleBoosting(float deltaTime) {
        if (currentState != GameState::Playing) {
            if (isBoosting) { isBoosting = false; boostCostTimer = 0.0f; boostCostCycles = 0; }
            return;
        }

        const Uint8* currentKeyStates = SDL_GetKeyboardState(nullptr);
        bool shiftPressed = currentKeyStates[SDL_SCANCODE_LSHIFT] || currentKeyStates[SDL_SCANCODE_RSHIFT];
        bool canBoost = shiftPressed && score > 0 && snake.getBody().size() > Config::MIN_BOOST_LENGTH;

        if (canBoost) {
            if (!isBoosting) {
                isBoosting = true; boostCostTimer = 0.0f; boostCostCycles = 0;
                std::cout << "Boost started." << std::endl;
                // PlaySoundEffect(boostStartSound.get(), soundEnabled); // Bỏ âm thanh bắt đầu boost
            }

            boostCostTimer += deltaTime;
            const float costIntervalSeconds = static_cast<float>(Config::BOOST_COST_INTERVAL_MS) / 1000.0f;

            while (isBoosting && boostCostTimer >= costIntervalSeconds) {
                boostCostTimer -= costIntervalSeconds;

                if (score > 0 && snake.getBody().size() > Config::MIN_BOOST_LENGTH) {
                    score -= Config::BOOST_SCORE_COST;
                    if (score < 0) score = 0;

                    boostCostCycles++;
                    if (boostCostCycles >= Config::BOOST_LENGTH_COST_INTERVALS) {
                        boostCostCycles = 0;
                        if (snake.getBody().size() > Config::MIN_BOOST_LENGTH) {
                            snake.shrink();
                            // PlaySoundEffect(shrinkSound.get(), soundEnabled); // Bỏ âm thanh shrink
                            std::cout << "Shrunk due to boost. New length: " << snake.getBody().size() << std::endl;
                        } else {
                            // PlaySoundEffect(boostEndSound.get(), soundEnabled); // Bỏ âm thanh dừng boost
                            isBoosting = false; std::cout << "Boost stopped: Cannot shrink further." << std::endl; boostCostTimer = 0.0f;
                        }
                    }
                    if (score <= 0 || snake.getBody().size() <= Config::MIN_BOOST_LENGTH) {
                        // PlaySoundEffect(boostEndSound.get(), soundEnabled); // Bỏ âm thanh dừng boost
                        isBoosting = false; std::cout << "Boost stopped due to running out of score/length." << std::endl; boostCostTimer = 0.0f;
                    }
                } else {
                    // PlaySoundEffect(boostEndSound.get(), soundEnabled); // Bỏ âm thanh dừng boost
                    isBoosting = false; std::cout << "Boost stopped: Conditions not met before applying cost." << std::endl; boostCostTimer = 0.0f;
                }
            }
        } else {
            if (isBoosting) {
                // PlaySoundEffect(boostEndSound.get(), soundEnabled); // Bỏ âm thanh dừng boost
                isBoosting = false; boostCostTimer = 0.0f; boostCostCycles = 0;
                std::cout << "Boost stopped (Shift released or conditions unmet)." << std::endl;
            }
        }
    }

    void Game::update() {
        if (currentState != GameState::Playing) return;

        SDL_Point nextHeadPos = snake.calculateNextHeadPosition();

        bool wallCollision = false;
        if (currentGameMode == GameMode::Classic) {
            wallCollision = (nextHeadPos.x < 0 || nextHeadPos.x >= screenWidth ||
                             nextHeadPos.y < 0 || nextHeadPos.y >= screenHeight);
        } else {
            if (nextHeadPos.x < 0) nextHeadPos.x = screenWidth - cellSize;
            else if (nextHeadPos.x >= screenWidth) nextHeadPos.x = 0;
            if (nextHeadPos.y < 0) nextHeadPos.y = screenHeight - cellSize;
            else if (nextHeadPos.y >= screenHeight) nextHeadPos.y = 0;
            wallCollision = false;
        }

        bool obstacleCollision = checkObstacleCollision(nextHeadPos);
        bool selfCollision = snake.checkSelfCollisionWithNext(nextHeadPos);

        if (wallCollision || obstacleCollision || selfCollision) {
            std::cout << "Collision! Game Over. Reason: ";
            if (wallCollision) std::cout << "Wall (Classic Mode).";
            else if (obstacleCollision) std::cout << "Obstacle (Hit by snake).";
            else if (selfCollision) std::cout << "Self.";
            std::cout << " Final Score: " << score << std::endl;
            if (isBoosting) { isBoosting = false; boostCostTimer = 0.0f; boostCostCycles = 0;}

            PlaySoundEffect(collisionSound.get(), soundEnabled);
            PlaySoundEffect(gameOverSound.get(), soundEnabled);

            if (isHighScore(score)) { currentState = GameState::EnteringHighScore; currentPlayerNameInput = ""; if(!isEnteringName) SDL_StartTextInput(); isEnteringName = true; }
            else { currentState = GameState::GameOver; if(isEnteringName) SDL_StopTextInput(); isEnteringName = false; }
            return;
        }

        snake.move(nextHeadPos);

        if (snake.checkFoodCollision(food.getPosition())) {
            PlaySoundEffect(eatSound.get(), soundEnabled);
            score++;
            snake.grow();
            placeFood();
            increaseSpeed();
            std::cout << "Ate food. Score: " << score << ", New speed interval: " << moveInterval << "ms" << std::endl;

            if (score >= nextObstacleScoreThreshold) {
                if (addSingleObstacle()) {
                    std::cout << "Added new obstacle at score " << score << "." << std::endl;
                    nextObstacleScoreThreshold += Config::OBSTACLE_ADD_SCORE_INTERVAL;
                } else {
                    nextObstacleScoreThreshold += Config::OBSTACLE_ADD_SCORE_INTERVAL;
                }
            }
        }
    }

    void Game::render(Renderer& renderer) {
        renderer.clear();
        switch (currentState) {
            case GameState::MainMenu:        renderMainMenu(renderer);       break;
            case GameState::Options:         renderOptions(renderer);        break;
            case GameState::Playing:
            case GameState::Paused:
            case GameState::GameOver:        renderGameScreen(renderer);     break;
            case GameState::EnteringHighScore: renderHighScoreEntry(renderer); break;
        }
        renderer.present();
    }

    void Game::renderMainMenu(Renderer& renderer) const {
        if (menuTexture) {
            SDL_Rect destRect = {0, 0, screenWidth, screenHeight};
            renderer.drawTexture(menuTexture.get(), &destRect);
            if (!menuButtons.empty() && selectedButtonIndex >= 0 && selectedButtonIndex < static_cast<int>(menuButtons.size())) {
                SDL_Rect highlightRect = menuButtons[selectedButtonIndex].screenRect;
                highlightRect.x -= 5; highlightRect.y -= 5; highlightRect.w += 10; highlightRect.h += 10;
                renderer.drawRect(&highlightRect, Config::MENU_HIGHLIGHT_COLOR, false);
            }
        } else {
            renderer.clear();
            renderer.renderCenteredText("Vorax Serpens", screenWidth, screenHeight / 4, Config::TEXT_COLOR);
            int btnY = screenHeight / 2; int spacing = Config::FONT_SIZE * 2;
            for(size_t i=0; i < menuButtons.size(); ++i) {
                SDL_Color c = (static_cast<int>(i) == selectedButtonIndex) ? Config::MENU_HIGHLIGHT_COLOR : Config::TEXT_COLOR;
                renderer.renderCenteredText(menuButtons[i].debugText, screenWidth, btnY + i * spacing, c);
            }
        }
        const int highScoreX = 20; const int highScoreY = 50;
        renderer.renderText("High Scores:", highScoreX, highScoreY, Config::TEXT_COLOR);
        int hs_y = highScoreY + Config::FONT_SIZE + 5; int rank = 1;
        for(const auto& entry : highScores) {
            std::string text = std::to_string(rank) + ". " + entry.name + ": " + std::to_string(entry.score);
            renderer.renderText(text, highScoreX, hs_y, Config::TEXT_COLOR);
            hs_y += Config::FONT_SIZE + 2; rank++;
            if (rank > maxHighScores) break;
        }
    }

    void Game::renderOptions(Renderer& renderer) {
        if (backgroundTexture) { SDL_Rect destRect = {0, 0, screenWidth, screenHeight}; renderer.drawTexture(backgroundTexture.get(), &destRect); }
        else { renderer.clear(); }
        renderer.renderCenteredText("OPTIONS", screenWidth, screenHeight / 5, Config::TEXT_COLOR);
        int startY = screenHeight / 3; int y_offset = Config::FONT_SIZE * 2 + 10;
        if (optionsMenuItemRects.size() != optionsMenuItems.size()) { optionsMenuItemRects.resize(optionsMenuItems.size()); }
        for (size_t i = 0; i < optionsMenuItems.size(); ++i) {
            const auto& item = optionsMenuItems[i];
            SDL_Color color = (static_cast<int>(i) == selectedOptionIndex) ? Config::OPTIONS_HIGHLIGHT_COLOR : Config::OPTIONS_TEXT_COLOR;
            std::string displayText = item.text;
            if (item.action == OptionAction::TOGGLE_MODE || item.action == OptionAction::TOGGLE_SOUND) { displayText = "< " + displayText + " >"; }
            SDL_Point textSize = renderer.getTextSize(displayText);
            SDL_Rect& currentRect = optionsMenuItemRects[i];
            currentRect.w = textSize.x; currentRect.h = textSize.y;
            currentRect.x = (screenWidth - currentRect.w) / 2; currentRect.y = startY + i * y_offset;
            renderer.renderText(displayText, currentRect.x, currentRect.y, color);
        }
        renderer.renderCenteredText("Use UP/DOWN/Click to navigate, ENTER/LEFT/RIGHT/Click to change/select", screenWidth, screenHeight - Config::FONT_SIZE * 4, Config::TEXT_COLOR);
        renderer.renderCenteredText("Press ESC to return to Main Menu", screenWidth, screenHeight - Config::FONT_SIZE * 3, Config::TEXT_COLOR);
    }

    void Game::renderGameScreen(Renderer& renderer) const {
        if (backgroundTexture) { SDL_Rect destRect = {0, 0, screenWidth, screenHeight}; renderer.drawTexture(backgroundTexture.get(), &destRect); }
        else { renderer.clear(); }
        if (!obstacles.empty()) {
            std::vector<SDL_Rect> obsRects; obsRects.reserve(obstacles.size());
            for (const auto& obs : obstacles) { obsRects.push_back({obs.position.x, obs.position.y, cellSize, cellSize}); }
            renderer.drawRects(obsRects, Config::OBSTACLE_COLOR, true);
        }
        SDL_Point foodPos = food.getPosition();
        if (foodTexture && foodPos.x >= 0 && foodPos.y >= 0) { SDL_Rect foodRect = {foodPos.x, foodPos.y, cellSize, cellSize}; renderer.drawTexture(foodTexture.get(), &foodRect); }
        else if (!foodTexture && foodPos.x >=0 && foodPos.y >=0) { SDL_Rect foodRect = {foodPos.x, foodPos.y, cellSize, cellSize}; renderer.drawRect(&foodRect, {255, 0, 0, 255}, true); }
        snake.draw(renderer.getSDLRenderer());
        int currentHighScore = highScores.empty() ? 0 : highScores[0].score;
        const_cast<Renderer&>(renderer).renderUI(score, currentHighScore, 10, 10, 10, 10 + Config::FONT_SIZE + 5, Config::TEXT_COLOR);
        if (isBoosting) { const_cast<Renderer&>(renderer).renderText("BOOST!", screenWidth - 100, 10, {255, 100, 0, 255}); }
        if (currentState == GameState::Paused && pausedTextTexture) {
            SDL_Rect destPausedRect = pausedTextRect; destPausedRect.x = (screenWidth - destPausedRect.w) / 2; destPausedRect.y = screenHeight / 2 - destPausedRect.h / 2;
            renderer.drawTexture(pausedTextTexture.get(), &destPausedRect);
            renderer.renderCenteredText("(Press P or Enter to Resume, ESC for Main Menu)", screenWidth, destPausedRect.y + destPausedRect.h + 10, Config::PAUSE_TEXT_COLOR);
        } else if (currentState == GameState::GameOver && gameOverTextTexture) {
            SDL_Rect destGameOverRect = gameOverTextRect; destGameOverRect.x = (screenWidth - destGameOverRect.w) / 2; destGameOverRect.y = screenHeight / 2 - destGameOverRect.h - 20;
            renderer.drawTexture(gameOverTextTexture.get(), &destGameOverRect);
            renderer.renderCenteredText("Press SPACE to Restart", screenWidth, destGameOverRect.y + destGameOverRect.h + 10, Config::GAMEOVER_TEXT_COLOR);
            renderer.renderCenteredText("Press ESC for Main Menu", screenWidth, destGameOverRect.y + destGameOverRect.h + 10 + Config::FONT_SIZE + 5, Config::GAMEOVER_TEXT_COLOR);
        }
    }

    void Game::renderHighScoreEntry(Renderer& renderer) const {
        renderGameScreen(renderer);
        SDL_Rect overlayRect = {0, 0, screenWidth, screenHeight}; renderer.drawRect(&overlayRect, {0, 0, 0, 150}, true);
        SDL_Rect inputBgRect = { screenWidth / 4, screenHeight / 3, screenWidth / 2, screenHeight / 3 }; renderer.drawRect(&inputBgRect, {50, 50, 50, 220}, true);
        int textY = inputBgRect.y + 30;
        renderer.renderCenteredText("New High Score!", screenWidth, textY, Config::MENU_HIGHLIGHT_COLOR);
        textY += Config::FONT_SIZE * 2; renderer.renderCenteredText("Enter Your Name:", screenWidth, textY, Config::TEXT_COLOR); textY += Config::FONT_SIZE * 2;
        std::string displayText = currentPlayerNameInput; Uint32 ticks = SDL_GetTicks();
        if ((ticks / 500) % 2 == 0) { displayText += '_'; } else { displayText += ' '; }
        renderer.renderCenteredText(displayText, screenWidth, textY, Config::TEXT_COLOR);
        renderer.renderCenteredText("(Max 15 chars, Enter to Confirm, ESC to Cancel)", screenWidth, inputBgRect.y + inputBgRect.h - 40, Config::PAUSE_TEXT_COLOR);
    }

    GameState Game::getCurrentState() const { return currentState; }

    bool Game::didQuit() const { return quitRequested; }

    void Game::increaseSpeed() {
        moveInterval = std::max(static_cast<Uint32>(Config::MIN_MOVE_INTERVAL_MS), moveInterval - static_cast<Uint32>(Config::SPEED_INCREMENT_MS));
    }

    void Game::generateObstacles() {
        obstacles.clear(); if (Config::OBSTACLE_COUNT <= 0) return;
        int maxGridX = std::max(0, (screenWidth / cellSize) - 1); int maxGridY = std::max(0, (screenHeight / cellSize) - 1);
        int gridArea = (maxGridX + 1) * (maxGridY + 1); if (gridArea <= 0) return;
        std::vector<SDL_Point> allPositions; allPositions.reserve(gridArea);
        for (int x = 0; x <= maxGridX; ++x) { for (int y = 0; y <= maxGridY; ++y) { allPositions.push_back({x * cellSize, y * cellSize}); } }
        std::vector<SDL_Point> invalidPositions; const auto& initialSnakeBody = snake.getBody(); SDL_Point startHead = initialSnakeBody.empty() ? SDL_Point{-1,-1} : initialSnakeBody.front();
        for (const auto& segment : initialSnakeBody) { invalidPositions.push_back(segment); }
        if (startHead.x != -1) {
            int safeRadius = 2;
            for (int dx = -safeRadius; dx <= safeRadius; ++dx) { for (int dy = -safeRadius; dy <= safeRadius; ++dy) {
                    SDL_Point safePos = {startHead.x + dx * cellSize, startHead.y + dy * cellSize};
                    if (safePos.x >= 0 && safePos.x < screenWidth && safePos.y >= 0 && safePos.y < screenHeight) {
                        bool alreadyInvalid = false;
                        for(const auto& invalid : invalidPositions) { if(invalid.x == safePos.x && invalid.y == safePos.y) { alreadyInvalid = true; break; } }
                        if (!alreadyInvalid) { invalidPositions.push_back(safePos); }
                    } } }
        }
        std::vector<SDL_Point> validObstaclePositions; validObstaclePositions.reserve(allPositions.size());
        for (const auto& p : allPositions) {
            bool isInvalid = false; for (const auto& invalid : invalidPositions) { if (p.x == invalid.x && p.y == invalid.y) { isInvalid = true; break; } }
            if (!isInvalid) { validObstaclePositions.push_back(p); }
        }
        std::shuffle(validObstaclePositions.begin(), validObstaclePositions.end(), rng);
        int count = std::min(static_cast<int>(validObstaclePositions.size()), Config::OBSTACLE_COUNT);
        if (count < Config::OBSTACLE_COUNT) { std::cout << "Warning: Could only place " << count << "/" << Config::OBSTACLE_COUNT << " initial obstacles due to space constraints." << std::endl; }
        if (count <= 0) { std::cout << "No valid positions to place initial obstacles." << std::endl; return; }

        std::uniform_real_distribution<float> moveTypeDist(0.0f, 1.0f);
        std::uniform_int_distribution<int> rangeDist(3, 8);
        std::uniform_int_distribution<int> dirDist(0, 1);

        float currentMovingRatio = std::min(Config::MAX_MOVING_OBSTACLE_RATIO,
                                            Config::BASE_MOVING_OBSTACLE_RATIO + score * Config::MOVING_RATIO_SCORE_FACTOR);
        int currentSpeedFactor = std::max(Config::MIN_OBSTACLE_SPEED_FACTOR,
                                          Config::BASE_OBSTACLE_SPEED_FACTOR - score / Config::OBSTACLE_SPEED_SCORE_DIVISOR);

        obstacles.reserve(count);
        for (int i = 0; i < count; ++i) {
            Obstacle obs;
            obs.position = validObstaclePositions[i];

            if (moveTypeDist(rng) < currentMovingRatio) {
                obs.movementType = (dirDist(rng) == 0) ? ObstacleMovement::Horizontal : ObstacleMovement::Vertical;
                obs.moveRange = rangeDist(rng);
                obs.currentMoveStep = 0;
                if (obs.movementType == ObstacleMovement::Horizontal) {
                    obs.moveDirection = (dirDist(rng) == 0) ? Direction::LEFT : Direction::RIGHT;
                } else {
                    obs.moveDirection = (dirDist(rng) == 0) ? Direction::UP : Direction::DOWN;
                }
                obs.moveSpeedFactor = currentSpeedFactor;
                obs.moveDelayCounter = rng() % obs.moveSpeedFactor;
            } else {
                obs.movementType = ObstacleMovement::Static;
            }
            obstacles.push_back(obs);
        }

        std::cout << "Generated " << obstacles.size() << " initial obstacles ("
                  << std::count_if(obstacles.begin(), obstacles.end(), [](const Obstacle& o){ return o.movementType != ObstacleMovement::Static; }) << " moving)."
                  << std::endl;
    }

    bool Game::addSingleObstacle() {
        int maxGridX = std::max(0, (screenWidth / cellSize) - 1); int maxGridY = std::max(0, (screenHeight / cellSize) - 1); int gridArea = (maxGridX + 1) * (maxGridY + 1);
        const auto& snakeBody = snake.getBody(); SDL_Point foodPos = food.getPosition();
        size_t occupiedSpots = snakeBody.size() + obstacles.size(); if (foodPos.x >= 0) occupiedSpots++;
        if (occupiedSpots >= static_cast<size_t>(gridArea) * 0.9) { return false; }
        std::uniform_int_distribution<int> distX(0, maxGridX); std::uniform_int_distribution<int> distY(0, maxGridY);
        SDL_Point potentialPos; bool validPosition; const int maxAttempts = gridArea + 50; int attempts = 0;
        do {
            potentialPos.x = distX(rng) * cellSize; potentialPos.y = distY(rng) * cellSize; validPosition = true; attempts++;
            for (const auto& segment : snakeBody) { if (potentialPos.x == segment.x && potentialPos.y == segment.y) { validPosition = false; break; } } if (!validPosition) continue;
            for (const auto& obs : obstacles) { if (potentialPos.x == obs.position.x && potentialPos.y == obs.position.y) { validPosition = false; break; } } if (!validPosition) continue;
            if (foodPos.x >= 0 && potentialPos.x == foodPos.x && potentialPos.y == foodPos.y) { validPosition = false; }
        } while (!validPosition && attempts < maxAttempts);
        if (!validPosition) { std::cerr << "Warning: Could not find a valid random position for new obstacle after " << maxAttempts << " attempts. Grid occupancy: " << occupiedSpots << "/" << gridArea << std::endl; return false; }

        Obstacle newObs;
        newObs.position = potentialPos;

        float currentMovingRatio = std::min(Config::MAX_MOVING_OBSTACLE_RATIO,
                                            Config::BASE_MOVING_OBSTACLE_RATIO + score * Config::MOVING_RATIO_SCORE_FACTOR);
        int currentSpeedFactor = std::max(Config::MIN_OBSTACLE_SPEED_FACTOR,
                                          Config::BASE_OBSTACLE_SPEED_FACTOR - score / Config::OBSTACLE_SPEED_SCORE_DIVISOR);

        std::uniform_real_distribution<float> moveTypeDist(0.0f, 1.0f);
        std::uniform_int_distribution<int> rangeDist(3, 6);
        std::uniform_int_distribution<int> dirDist(0, 1);

        if (moveTypeDist(rng) < currentMovingRatio) {
            newObs.movementType = (dirDist(rng) == 0) ? ObstacleMovement::Horizontal : ObstacleMovement::Vertical;
            newObs.moveRange = rangeDist(rng);
            newObs.currentMoveStep = 0;
            if (newObs.movementType == ObstacleMovement::Horizontal) {
                newObs.moveDirection = (dirDist(rng) == 0) ? Direction::LEFT : Direction::RIGHT;
            } else {
                newObs.moveDirection = (dirDist(rng) == 0) ? Direction::UP : Direction::DOWN;
            }
            newObs.moveSpeedFactor = currentSpeedFactor;
            newObs.moveDelayCounter = rng() % newObs.moveSpeedFactor;
        } else {
            newObs.movementType = ObstacleMovement::Static;
        }

        obstacles.push_back(newObs);
        return true;
    }


    bool Game::checkObstacleCollision(const SDL_Point& pos) const {
        return std::any_of(obstacles.begin(), obstacles.end(), [&](const Obstacle& obs){ return pos.x == obs.position.x && pos.y == obs.position.y; });
    }

    void Game::placeFood() {
        std::vector<SDL_Point> currentObstaclePositions; currentObstaclePositions.reserve(obstacles.size());
        for(const auto& obs : obstacles) { currentObstaclePositions.push_back(obs.position); }
        food.generate(screenWidth, screenHeight, snake.getBody(), currentObstaclePositions);
        if (food.getPosition().x < 0) { std::cerr << "Error: Failed to place food on the grid! The grid might be full." << std::endl; }
    }

    void Game::updateObstacles() {
        const auto& snakeBody = snake.getBody(); SDL_Point currentFoodPos = food.getPosition();
        for (auto& obs : obstacles) {
            if (currentGameMode == GameMode::PortalWalls && obs.movementType != ObstacleMovement::Static)
            {
                obs.moveDelayCounter++; if (obs.moveDelayCounter < obs.moveSpeedFactor) { continue; } obs.moveDelayCounter = 0;
                SDL_Point nextPos = obs.position; int dx = 0, dy = 0;
                switch (obs.moveDirection) { case Direction::UP: dy = -cellSize; break; case Direction::DOWN: dy = cellSize; break; case Direction::LEFT: dx = -cellSize; break; case Direction::RIGHT: dx = cellSize; break; }
                nextPos.x += dx; nextPos.y += dy;
                if (nextPos.x < 0) nextPos.x = screenWidth - cellSize; else if (nextPos.x >= screenWidth) nextPos.x = 0; if (nextPos.y < 0) nextPos.y = screenHeight - cellSize; else if (nextPos.y >= screenHeight) nextPos.y = 0;
                bool collisionDetected = false;
                for (const auto& otherObs : obstacles) { if (&obs == &otherObs) continue; if (nextPos.x == otherObs.position.x && nextPos.y == otherObs.position.y) { collisionDetected = true; break; } }
                if (!collisionDetected) { for (size_t i = 0; i < snakeBody.size(); ++i) { if (nextPos.x == snakeBody[i].x && nextPos.y == snakeBody[i].y) { collisionDetected = true; break; } } }
                if (currentFoodPos.x >= 0 && nextPos.x == currentFoodPos.x && nextPos.y == currentFoodPos.y) {
                    if (!collisionDetected) { std::cout << "Obstacle moving into food! Generating new food." << std::endl; placeFood(); currentFoodPos = food.getPosition(); }
                }
                if (collisionDetected) {
                    if (obs.movementType == ObstacleMovement::Horizontal) { obs.moveDirection = (obs.moveDirection == Direction::LEFT) ? Direction::RIGHT : Direction::LEFT; }
                    else { obs.moveDirection = (obs.moveDirection == Direction::UP) ? Direction::DOWN : Direction::UP; }
                    obs.currentMoveStep = 0;
                } else {
                    obs.position = nextPos; obs.currentMoveStep++;
                    if (obs.currentMoveStep >= obs.moveRange) {
                        if (obs.movementType == ObstacleMovement::Horizontal) { obs.moveDirection = (obs.moveDirection == Direction::LEFT) ? Direction::RIGHT : Direction::LEFT; }
                        else { obs.moveDirection = (obs.moveDirection == Direction::UP) ? Direction::DOWN : Direction::UP; }
                        obs.currentMoveStep = 0;
                    } } } }
    }

    void Game::reset() {
        std::cout << "Resetting game state for mode: " << (currentGameMode == GameMode::Classic ? "Classic" : "Portal") << std::endl;
        SDL_Point startPos = calculateStartPosition();
        snake = Snake(startPos.x, startPos.y, cellSize, Config::DEFAULT_SNAKE_LENGTH);
        score = 0; moveInterval = Config::INITIAL_SNAKE_SPEED_DELAY_MS; timeAccumulator = 0.0f; nextObstacleScoreThreshold = Config::OBSTACLE_ADD_SCORE_INTERVAL;
        obstacles.clear(); generateObstacles(); placeFood();
        isBoosting = false; boostCostTimer = 0.0f; boostCostCycles = 0;
        selectedButtonIndex = 0; selectedOptionIndex = 0; for(auto& rect : optionsMenuItemRects) { rect = {0,0,0,0}; }
        currentState = GameState::Playing;
        if(isEnteringName) { SDL_StopTextInput(); isEnteringName = false; } currentPlayerNameInput = "";
        std::cout << "Game reset complete. Ready to play." << std::endl;
    }

    void Game::loadHighScores() {
        highScores.clear(); std::ifstream file(Config::HIGHSCORE_FILE); std::cout << "Attempting to load high scores from: " << Config::HIGHSCORE_FILE << std::endl;
        if (file.is_open()) {
            std::string line; while (std::getline(file, line)) {
                if (line.find_first_not_of(" \t\n\r\f\v") == std::string::npos) { continue; }
                std::stringstream ss(line); int loadedScore = -1; std::string name = "";
                if (ss >> loadedScore) { std::getline(ss >> std::ws, name); }
                if (loadedScore >= 0 && !name.empty()) { highScores.push_back({name, loadedScore}); }
                else { std::cerr << "Warning: Invalid line format in highscore file: '" << line << "'" << std::endl; } }
            file.close(); std::sort(highScores.begin(), highScores.end());
            if (highScores.size() > static_cast<size_t>(maxHighScores)) { highScores.resize(maxHighScores); }
            std::cout << "Successfully loaded " << highScores.size() << " high scores." << std::endl;
        } else { std::cout << "High score file not found or could not be opened ('" << Config::HIGHSCORE_FILE << "'). Starting with empty scores." << std::endl; }
    }

    void Game::saveHighScores() const {
        std::cout << "Attempting to save " << highScores.size() << " high scores to: " << Config::HIGHSCORE_FILE << std::endl;
        std::ofstream file(Config::HIGHSCORE_FILE, std::ios::trunc);
        if (file.is_open()) {
            for (const auto& entry : highScores) { file << entry.score << " " << entry.name << std::endl; }
            file.close();
            if (file.fail()) { std::cerr << "Error: Failed to write all high scores to " << Config::HIGHSCORE_FILE << ". Check permissions or disk space." << std::endl; }
            else { std::cout << "High scores saved successfully." << std::endl; }
        } else { std::cerr << "Error: Could not open " << Config::HIGHSCORE_FILE << " for writing high scores." << std::endl; }
    }

    bool Game::isHighScore(int currentScore) const {
        if (currentScore <= 0) return false;
        if (highScores.size() < static_cast<size_t>(maxHighScores)) { return true; }
        return currentScore > highScores.back().score;
    }

    void Game::addHighScore(const std::string& name, int score) {
        std::string trimmedName = name; size_t first = trimmedName.find_first_not_of(" \t\n\r\f\v");
        if (std::string::npos == first) { trimmedName = "Player"; }
        else { size_t last = trimmedName.find_last_not_of(" \t\n\r\f\v"); trimmedName = trimmedName.substr(first, (last - first + 1)); }
        if (trimmedName.empty()) trimmedName = "Player";
        highScores.push_back({trimmedName, score}); std::sort(highScores.begin(), highScores.end());
        if (highScores.size() > static_cast<size_t>(maxHighScores)) { highScores.resize(maxHighScores); }
        std::cout << "Added high score: " << trimmedName << " - " << score << ". High score list size: " << highScores.size() << std::endl;
    }

}