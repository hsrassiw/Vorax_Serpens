#ifndef RENDERER_HPP
#define RENDERER_HPP

#include <SDL.h>

class Renderer {
public:
    explicit Renderer(SDL_Window* window);
    ~Renderer();

    SDL_Renderer* getRenderer() const;
    void clear() const;
    void present() const;

private:
    SDL_Renderer* renderer;
};

#endif
