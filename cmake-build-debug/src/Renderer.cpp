#include "Renderer.hpp"
#include "Config.hpp"
#include <SDL_ttf.h>
#include <iostream>
#include <utility>

Renderer::Renderer(SDL_Window* window, const std::string& fontPath, int fontSize) {
    if (!window) {
        throw RendererError("Window pointer is null during Renderer creation.");
    }
    sdlRenderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!sdlRenderer) {
        throw RendererError("Failed to create SDL_Renderer: " + std::string(SDL_GetError()));
    }

    font = TTF_OpenFont(fontPath.c_str(), fontSize);
    if (!font) {
        std::cerr << "Warning: Failed to load font '" << fontPath << "': " << TTF_GetError() << std::endl;
    }

    SDL_SetRenderDrawColor(sdlRenderer, Config::BACKGROUND_COLOR.r, Config::BACKGROUND_COLOR.g, Config::BACKGROUND_COLOR.b, Config::BACKGROUND_COLOR.a);
}

Renderer::~Renderer() {
    cleanup();
}

Renderer::Renderer(Renderer&& other) noexcept
        : sdlRenderer(other.sdlRenderer), font(other.font) {
    other.sdlRenderer = nullptr;
    other.font = nullptr;
}

Renderer& Renderer::operator=(Renderer&& other) noexcept {
    if (this != &other) {
        cleanup();
        sdlRenderer = other.sdlRenderer;
        font = other.font;
        other.sdlRenderer = nullptr;
        other.font = nullptr;
    }
    return *this;
}

void Renderer::cleanup() {
    if (font) {
        TTF_CloseFont(font);
        font = nullptr;
    }
    if (sdlRenderer) {
        SDL_DestroyRenderer(sdlRenderer);
        sdlRenderer = nullptr;
    }
}

SDL_Renderer* Renderer::getSDLRenderer() const {
    return sdlRenderer;
}

TTF_Font* Renderer::getFont() const {
    return font;
}

void Renderer::clear() const {
    if (!sdlRenderer) return;
    SDL_SetRenderDrawColor(sdlRenderer, Config::BACKGROUND_COLOR.r, Config::BACKGROUND_COLOR.g, Config::BACKGROUND_COLOR.b, Config::BACKGROUND_COLOR.a);
    SDL_RenderClear(sdlRenderer);
}

void Renderer::present() const {
    if (!sdlRenderer) return;
    SDL_RenderPresent(sdlRenderer);
}

void Renderer::renderText(const std::string& text, int x, int y, SDL_Color color) const {
    if (!sdlRenderer || !font || text.empty()) {
        return;
    }

    SDL_Surface* textSurface = TTF_RenderText_Solid(font, text.c_str(), color);
    if (!textSurface) {
        std::cerr << "Warning: Unable to render text surface! TTF_Error: " << TTF_GetError() << std::endl;
        return;
    }

    SDL_Texture* textTexture = SDL_CreateTextureFromSurface(sdlRenderer, textSurface);
    if (!textTexture) {
        std::cerr << "Warning: Unable to create texture from rendered text! SDL_Error: " << SDL_GetError() << std::endl;
        SDL_FreeSurface(textSurface);
        return;
    }

    SDL_Rect renderQuad = {x, y, textSurface->w, textSurface->h};
    SDL_FreeSurface(textSurface);
    SDL_RenderCopy(sdlRenderer, textTexture, nullptr, &renderQuad);
    SDL_DestroyTexture(textTexture);
}

SDL_Point Renderer::getTextSize(const std::string& text) const {
    if (!font || text.empty()) {
        return {0, 0};
    }
    int width = 0;
    int height = 0;
    if (TTF_SizeText(font, text.c_str(), &width, &height) != 0) {
        std::cerr << "Warning: Unable to get text size! TTF_Error: " << TTF_GetError() << std::endl;
        return {0, 0};
    }
    return {width, height};
}