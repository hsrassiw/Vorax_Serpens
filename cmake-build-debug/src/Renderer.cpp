#include "Renderer.hpp"
#include "Config.hpp"
#include <iostream>
#include <utility>
#include <cstring>

namespace SnakeGame {

    Renderer::Renderer(SDL_Window* window, const std::string& fontPath, int fontSize) {
        if (!window) {
            throw RendererError("Window pointer is null during Renderer creation.");
        }

        sdlRenderer.reset(SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC));
        if (!sdlRenderer) {
            throw RendererError("Failed to create SDL_Renderer: " + std::string(SDL_GetError()));
        }

        font.reset(TTF_OpenFont(fontPath.c_str(), fontSize));
        if (!font) {
            std::cerr << "Warning: Failed to load font '" << fontPath << "': " << TTF_GetError() << ". Text rendering will fail." << std::endl;
        }

        SDL_SetRenderDrawColor(getSDLRenderer(), Config::BACKGROUND_COLOR.r, Config::BACKGROUND_COLOR.g, Config::BACKGROUND_COLOR.b, Config::BACKGROUND_COLOR.a);
    }

    SDL_Renderer* Renderer::getSDLRenderer() const {
        return sdlRenderer.get();
    }

    void Renderer::clear() const {
        if (!sdlRenderer) return;
        SDL_SetRenderDrawColor(getSDLRenderer(), Config::BACKGROUND_COLOR.r, Config::BACKGROUND_COLOR.g, Config::BACKGROUND_COLOR.b, Config::BACKGROUND_COLOR.a);
        SDL_RenderClear(getSDLRenderer());
    }

    void Renderer::present() const {
        if (!sdlRenderer) return;
        SDL_RenderPresent(getSDLRenderer());
    }

    SDL_Texture* Renderer::createTextTexture(const std::string& text, SDL_Color color) const {
        if (!font || text.empty() || !sdlRenderer) {
            return nullptr;
        }

        SDL_Surface* textSurface = TTF_RenderText_Solid(font.get(), text.c_str(), color);
        if (!textSurface) {
            std::cerr << "Warning: Unable to render text surface! TTF_Error: " << TTF_GetError() << std::endl;
            return nullptr;
        }

        SDL_Texture* textTexture = SDL_CreateTextureFromSurface(getSDLRenderer(), textSurface);
        SDL_FreeSurface(textSurface);

        if (!textTexture) {
            std::cerr << "Warning: Unable to create texture from rendered text! SDL_Error: " << SDL_GetError() << std::endl;
        }

        return textTexture;
    }

    void Renderer::renderUI(int score, int highScore, int scoreX, int scoreY, int hsX, int hsY, SDL_Color textColor) {
        if (!sdlRenderer) return;

        if (score != cachedScore || memcmp(&textColor, &scoreColor, sizeof(SDL_Color)) != 0) {
            cachedScore = score;
            scoreColor = textColor;
            std::string scoreText = "Score: " + std::to_string(score);
            SDL_Texture* newTexture = createTextTexture(scoreText, textColor);
            scoreTexture.reset(newTexture);

            if (scoreTexture) {
                SDL_QueryTexture(scoreTexture.get(), nullptr, nullptr, &scoreRect.w, &scoreRect.h);
                scoreRect.x = scoreX;
                scoreRect.y = scoreY;
            } else {
                scoreRect = {0,0,0,0};
            }
        }
        if (scoreTexture) {
            SDL_RenderCopy(getSDLRenderer(), scoreTexture.get(), nullptr, &scoreRect);
        }

        if (highScore != cachedHighScore || memcmp(&textColor, &highScoreColor, sizeof(SDL_Color)) != 0) {
            cachedHighScore = highScore;
            highScoreColor = textColor;
            std::string hsText = "High Score: " + std::to_string(highScore);
            SDL_Texture* newTexture = createTextTexture(hsText, textColor);
            highScoreTexture.reset(newTexture);

            if (highScoreTexture) {
                SDL_QueryTexture(highScoreTexture.get(), nullptr, nullptr, &highScoreRect.w, &highScoreRect.h);
                highScoreRect.x = hsX;
                highScoreRect.y = hsY;
            } else {
                highScoreRect = {0,0,0,0};
            }
        }
        if (highScoreTexture) {
            SDL_RenderCopy(getSDLRenderer(), highScoreTexture.get(), nullptr, &highScoreRect);
        }
    }

    void Renderer::renderCenteredText(const std::string& text, int screenWidth, int screenHeight, SDL_Color color) const {
        if (text.empty() || !sdlRenderer) return;

        std::unique_ptr<SDL_Texture, SDLTextureDestroyer> tempTexture(createTextTexture(text, color));

        if (!tempTexture) {
            return;
        }

        int textW = 0, textH = 0;
        if (SDL_QueryTexture(tempTexture.get(), nullptr, nullptr, &textW, &textH) != 0) {
            std::cerr << "Warning: Could not query texture for centered text! SDL_Error: " << SDL_GetError() << std::endl;
            return;
        }

        SDL_Rect renderQuad = { (screenWidth - textW) / 2, (screenHeight - textH) / 2, textW, textH };

        SDL_RenderCopy(getSDLRenderer(), tempTexture.get(), nullptr, &renderQuad);
    }

    SDL_Point Renderer::getTextSize(const std::string& text) const {
        if (!font || text.empty()) {
            return {0, 0};
        }
        int width = 0;
        int height = 0;
        if (TTF_SizeText(font.get(), text.c_str(), &width, &height) != 0) {
            std::cerr << "Warning: Unable to get text size! TTF_Error: " << TTF_GetError() << std::endl;
            return {0, 0};
        }
        return {width, height};
    }

}