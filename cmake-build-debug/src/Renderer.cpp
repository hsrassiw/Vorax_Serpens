#include "Renderer.hpp"
#include "Config.hpp"
#include <SDL_image.h>
#include <iostream>
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

    SDL_Texture* Renderer::loadTexture(const std::string& path) const {
        if (!sdlRenderer) return nullptr;
        SDL_Texture* newTexture = IMG_LoadTexture(sdlRenderer.get(), path.c_str());
        if (!newTexture) {
            std::cerr << "Warning: Unable to load texture " << path << "! IMG_Error: " << IMG_GetError() << std::endl;
        }
        return newTexture;
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

    void Renderer::drawTexture(SDL_Texture* texture, const SDL_Rect* destRect) const {
        if (!texture || !sdlRenderer || !destRect) return;
        SDL_RenderCopy(sdlRenderer.get(), texture, nullptr, destRect);
    }

    void Renderer::drawTexturePortion(SDL_Texture* texture, const SDL_Rect* srcRect, const SDL_Rect* destRect) const {
        if (!texture || !sdlRenderer || !srcRect || !destRect) return;
        SDL_RenderCopy(sdlRenderer.get(), texture, srcRect, destRect);
    }

    void Renderer::drawRect(const SDL_Rect* rect, SDL_Color color, bool filled) const {
        if (!rect || !sdlRenderer) return;
        SDL_SetRenderDrawColor(sdlRenderer.get(), color.r, color.g, color.b, color.a);
        if (filled) {
            SDL_RenderFillRect(sdlRenderer.get(), rect);
        } else {
            SDL_RenderDrawRect(sdlRenderer.get(), rect);
        }
    }

    void Renderer::drawRects(const std::vector<SDL_Rect>& rects, SDL_Color color, bool filled) const {
        if (rects.empty() || !sdlRenderer) return;
        SDL_SetRenderDrawColor(sdlRenderer.get(), color.r, color.g, color.b, color.a);
        if (filled) {
            SDL_RenderFillRects(sdlRenderer.get(), rects.data(), static_cast<int>(rects.size()));
        } else {
            SDL_RenderDrawRects(sdlRenderer.get(), rects.data(), static_cast<int>(rects.size()));
        }
    }

    SDL_Texture* Renderer::createTextTexture(const std::string& text, SDL_Color color) const {
        if (!font || text.empty() || !sdlRenderer) {
            if (!font) std::cerr << "Warning: Attempted to create text texture with no font loaded.\n";
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
            scoreTexture.reset(createTextTexture(scoreText, textColor));
            if (scoreTexture) {
                SDL_QueryTexture(scoreTexture.get(), nullptr, nullptr, &scoreRect.w, &scoreRect.h);
                scoreRect.x = scoreX; scoreRect.y = scoreY;
            } else { scoreRect = {0,0,0,0}; }
        }
        if (scoreTexture) { SDL_RenderCopy(getSDLRenderer(), scoreTexture.get(), nullptr, &scoreRect); }

        if (highScore != cachedHighScore || memcmp(&textColor, &highScoreColor, sizeof(SDL_Color)) != 0) {
            cachedHighScore = highScore;
            highScoreColor = textColor;
            std::string hsText = "High Score: " + std::to_string(highScore);
            highScoreTexture.reset(createTextTexture(hsText, textColor));
            if (highScoreTexture) {
                SDL_QueryTexture(highScoreTexture.get(), nullptr, nullptr, &highScoreRect.w, &highScoreRect.h);
                highScoreRect.x = hsX; highScoreRect.y = hsY;
            } else { highScoreRect = {0,0,0,0}; }
        }
        if (highScoreTexture) { SDL_RenderCopy(getSDLRenderer(), highScoreTexture.get(), nullptr, &highScoreRect); }
    }

    void Renderer::renderCenteredText(const std::string& text, int screenWidth, int screenHeight, SDL_Color color) const {
        if (text.empty() || !sdlRenderer) return;
        std::unique_ptr<SDL_Texture, SDLTextureDestroyer> tempTexture(createTextTexture(text, color));
        if (!tempTexture) { return; }
        int textW = 0, textH = 0;
        if (SDL_QueryTexture(tempTexture.get(), nullptr, nullptr, &textW, &textH) != 0) {
            std::cerr << "Warning: Could not query texture for centered text! SDL_Error: " << SDL_GetError() << std::endl;
            return;
        }
        SDL_Rect renderQuad = { (screenWidth - textW) / 2, (screenHeight - textH) / 2, textW, textH };
        SDL_RenderCopy(getSDLRenderer(), tempTexture.get(), nullptr, &renderQuad);
    }

    SDL_Point Renderer::getTextSize(const std::string& text) const {
        if (!font || text.empty()) { return {0, 0}; }
        int width = 0; int height = 0;
        if (TTF_SizeText(font.get(), text.c_str(), &width, &height) != 0) {
            std::cerr << "Warning: Unable to get text size! TTF_Error: " << TTF_GetError() << std::endl;
            return {0, 0};
        }
        return {width, height};
    }

}