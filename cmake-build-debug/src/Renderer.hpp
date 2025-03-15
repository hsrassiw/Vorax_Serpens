#ifndef RENDERER_HPP
#define RENDERER_HPP

#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>
#include<string>

class Renderer {
public:
    explicit Renderer(SDL_Window* window);
    ~Renderer();

    SDL_Renderer* getRenderer() const;
    void clear() const;
    void present() const;
    void renderText(const std::string& text, int x, int y, SDL_Color color) const;


private:
    SDL_Renderer* renderer;
};

#endif
