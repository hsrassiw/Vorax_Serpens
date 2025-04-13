#ifndef GAME_HPP
#define GAME_HPP

#include "Snake.hpp"
#include "Food.hpp"
#include "Renderer.hpp"
#include "Config.hpp"
#include <SDL.h>
#include <SDL_mixer.h>
#include <string>
#include <vector>
#include <memory>

namespace SnakeGame {

    enum class GameState { MainMenu, Options, Playing, Paused, GameOver };

    struct MixChunkDeleter {
        void operator()(Mix_Chunk* chunk) const {
            if (chunk) Mix_FreeChunk(chunk);
        }
    };

    struct MenuButton {
        SDL_Rect screenRect;
        SDL_Rect sourceRect;
        GameState targetState;
        bool requestsQuit = false;
    };

    class Game {
    public:
        Game(int screenWidth, int screenHeight, int cellSize, Renderer& renderer);
        ~Game() = default;

        void handleInput(const SDL_Event& event);
        void update();
        void render(Renderer& renderer) const;
        void runFrame(float deltaTime);

        [[nodiscard]] GameState getCurrentState() const;
        [[nodiscard]] int getScore() const { return score; }
        [[nodiscard]] int getHighScore() const { return highScore; }
        [[nodiscard]] bool didQuit() const;

        void reset();

    private:
        int screenWidth;
        int screenHeight;
        int cellSize;

        Snake snake;
        Food food;

        GameState currentState;
        int score;
        int highScore;
        Uint32 moveInterval;
        bool quitRequested;

        float timeAccumulator;

        std::unique_ptr<SDL_Texture, SDLTextureDestroyer> menuTexture;
        std::unique_ptr<SDL_Texture, SDLTextureDestroyer> backgroundTexture;

        std::unique_ptr<SDL_Texture, SDLTextureDestroyer> foodTexture;
        std::unique_ptr<Mix_Chunk, MixChunkDeleter> eatSound;

        SDL_Rect logoDestRect;
        std::vector<MenuButton> menuButtons;
        int selectedButtonIndex;

        void initAssets(Renderer& renderer);

        SDL_Point calculateStartPosition() const;
        void increaseSpeed();
        void placeFood();

        int loadHighScore();
        void saveHighScore() const;
    };

}

#endif