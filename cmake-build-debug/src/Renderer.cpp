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
