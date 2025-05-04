#include "Renderer.hpp"
#include "Config.hpp"
#include <SDL_image.h>
#include <iostream>
#include <cstring>

namespace SnakeGame {

    constexpr int FONT_RENDER_SCALE = 3;

    Renderer::Renderer(SDL_Window* window, const std::string& fontPath, int fontSize)
            :   cachedScore(-1), cachedHighScore(-1),
                scoreRect{0,0,0,0}, highScoreRect{0,0,0,0}, textRectCache{0,0,0,0},
                scoreColor{0,0,0,0}, highScoreColor{0,0,0,0}
    {
        if (!window) {
            throw RendererError("Window pointer is null during Renderer creation.");
        }

        sdlRenderer.reset(SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC));
        if (!sdlRenderer) {
            throw RendererError("Failed to create SDL_Renderer: " + std::string(SDL_GetError()));
        }

        int renderFontSize = fontSize * FONT_RENDER_SCALE;
        font.reset(TTF_OpenFont(fontPath.c_str(), renderFontSize));
        if (!font) {
            throw RendererError("Failed to load font '" + fontPath + "' at render size " + std::to_string(renderFontSize) + ": " + TTF_GetError());
        } else {
            std::cout << "Font '" << fontPath << "' loaded successfully at render size " << renderFontSize << " (target display size " << fontSize << ")" << std::endl;
        }

        SDL_SetRenderDrawColor(getSDLRenderer(), Config::BACKGROUND_COLOR.r, Config::BACKGROUND_COLOR.g, Config::BACKGROUND_COLOR.b, Config::BACKGROUND_COLOR.a);
    }

    SDL_Renderer* Renderer::getSDLRenderer() const {
        return sdlRenderer.get();
    }

    SDL_Texture* Renderer::loadTexture(const std::string& path) const {
        if (!sdlRenderer) {
            std::cerr << "Error: Cannot load texture, SDL_Renderer is null." << std::endl;
            return nullptr;
        }
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
            return nullptr;
        }

        SDL_Surface* textSurface = TTF_RenderText_Blended(font.get(), text.c_str(), color);
        if (!textSurface) {
            std::cerr << "Warning: TTF_RenderText_Blended failed! Text: \"" << text << "\" TTF_Error: " << TTF_GetError() << std::endl;
            return nullptr;
        }

        SDL_Texture* textTexture = SDL_CreateTextureFromSurface(getSDLRenderer(), textSurface);
        SDL_FreeSurface(textSurface);

        if (!textTexture) {
            std::cerr << "Warning: SDL_CreateTextureFromSurface failed! SDL_Error: " << SDL_GetError() << std::endl;
            return nullptr;
        }

        SDL_SetTextureBlendMode(textTexture, SDL_BLENDMODE_BLEND);

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
                int texW, texH;
                queryTexture(scoreTexture.get(), nullptr, nullptr, &texW, &texH);
                scoreRect.w = texW / FONT_RENDER_SCALE;
                scoreRect.h = texH / FONT_RENDER_SCALE;
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
            highScoreTexture.reset(createTextTexture(hsText, textColor));
            if (highScoreTexture) {
                int texW, texH;
                queryTexture(highScoreTexture.get(), nullptr, nullptr, &texW, &texH);
                highScoreRect.w = texW / FONT_RENDER_SCALE;
                highScoreRect.h = texH / FONT_RENDER_SCALE;
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

    void Renderer::renderCenteredText(const std::string& text, int screenWidth, int yPos, SDL_Color color) {
        if (text.empty() || !sdlRenderer || !font) return;

        textTextureCache.reset(createTextTexture(text, color));
        if (!textTextureCache) { return; }

        int texW = 0, texH = 0;
        if (queryTexture(textTextureCache.get(), nullptr, nullptr, &texW, &texH) == 0) {
            int targetW = texW / FONT_RENDER_SCALE;
            int targetH = texH / FONT_RENDER_SCALE;
            SDL_Rect renderQuad = { (screenWidth - targetW) / 2, yPos, targetW, targetH };
            SDL_RenderCopy(getSDLRenderer(), textTextureCache.get(), nullptr, &renderQuad);
        } else {
            std::cerr << "Warning: Could not query texture size for centered text! SDL_Error: " << SDL_GetError() << std::endl;
        }
    }

    void Renderer::renderText(const std::string& text, int x, int y, SDL_Color color) {
        if (text.empty() || !sdlRenderer || !font) return;

        textTextureCache.reset(createTextTexture(text, color));
        if (!textTextureCache) return;

        int texW = 0, texH = 0;
        if (queryTexture(textTextureCache.get(), nullptr, nullptr, &texW, &texH) == 0) {
            textRectCache.w = texW / FONT_RENDER_SCALE;
            textRectCache.h = texH / FONT_RENDER_SCALE;
            textRectCache.x = x;
            textRectCache.y = y;
            SDL_RenderCopy(getSDLRenderer(), textTextureCache.get(), nullptr, &textRectCache);
        } else {
            std::cerr << "Warning: Could not query texture size for simple text! SDL_Error: " << SDL_GetError() << std::endl;
        }
    }

    SDL_Point Renderer::getTextSize(const std::string& text) const {
        if (!font || text.empty()) { return {0, 0}; }
        int width = 0;
        int height = 0;
        if (TTF_SizeText(font.get(), text.c_str(), &width, &height) != 0) {
            std::cerr << "Warning: Unable to get scaled text size! Text: \"" << text << "\" TTF_Error: " << TTF_GetError() << std::endl;
            return {0, 0};
        }
        return {width / FONT_RENDER_SCALE, height / FONT_RENDER_SCALE};
    }

    int Renderer::queryTexture(SDL_Texture* texture, Uint32* format, int* access, int* w, int* h) const {
        if(!texture) {
            if (w) *w = 0;
            if (h) *h = 0;
            return -1;
        }
        return SDL_QueryTexture(texture, format, access, w, h);
    }

}
