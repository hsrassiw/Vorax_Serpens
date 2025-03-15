#include "Renderer.hpp"
#include <iostream>

Renderer::Renderer(SDL_Window* window) {
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) std::cerr << "Renderer creation failed: " << SDL_GetError() << std::endl;
}

Renderer::~Renderer() {
    if (renderer) SDL_DestroyRenderer(renderer);
}

SDL_Renderer* Renderer::getRenderer() const { return renderer; }

void Renderer::clear() const {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
}

void Renderer::present() const {
    SDL_RenderPresent(renderer);
}


void Renderer::renderText(const std::string& text, int x, int y, SDL_Color color) const {
    TTF_Font* font = TTF_OpenFont("assets/fonts/arial.ttf", 24);
    if (!font) return;

    SDL_Surface* textSurface = TTF_RenderText_Solid(font, text.c_str(), color);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, textSurface);

    SDL_Rect textRect = {x, y, textSurface->w, textSurface->h};
    SDL_RenderCopy(renderer, texture, nullptr, &textRect);

    SDL_FreeSurface(textSurface);
    SDL_DestroyTexture(texture);
    TTF_CloseFont(font);
}
