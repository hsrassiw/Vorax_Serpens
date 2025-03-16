#ifndef RENDERER_HPP
#define RENDERER_HPP

#include <SDL.h>
#include<string>

class Renderer {
public:
    explicit Renderer(SDL_Window* window);
    ~Renderer();

    [[nodiscard]] SDL_Renderer* getRenderer() const;
    void clear() const;
    void present() const;
    void renderText(const std::string& text, int x, int y, SDL_Color color) const;


private:
    SDL_Renderer* renderer;
};

#endif
