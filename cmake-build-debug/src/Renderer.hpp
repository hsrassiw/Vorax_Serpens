#ifndef RENDERER_HPP
#define RENDERER_HPP

#include <SDL.h>
#include <SDL_ttf.h>
#include <string>
#include <stdexcept>

class Renderer {
public:
    explicit Renderer(SDL_Window* window, const std::string& fontPath, int fontSize);
    ~Renderer();

    Renderer(const Renderer&) = delete;
    Renderer& operator=(const Renderer&) = delete;
    Renderer(Renderer&&) noexcept;
    Renderer& operator=(Renderer&&) noexcept;


    [[nodiscard]] SDL_Renderer* getSDLRenderer() const;
    [[nodiscard]] TTF_Font* getFont() const;

    void clear() const;
    void present() const;
    void renderText(const std::string& text, int x, int y, SDL_Color color) const;
    SDL_Point getTextSize(const std::string& text) const;

private:
    SDL_Renderer* sdlRenderer = nullptr;
    TTF_Font* font = nullptr;

    void cleanup();
};

class RendererError : public std::runtime_error {
public:
    explicit RendererError(const std::string& message) : std::runtime_error(message) {}
};


#endif