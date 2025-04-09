#ifndef RENDERER_HPP
#define RENDERER_HPP

#include <SDL.h>
#include <SDL_ttf.h>
#include <string>
#include <stdexcept>
#include <memory>

namespace SnakeGame {

    struct SDLWindowDestroyer { void operator()(SDL_Window* w) const { if(w) SDL_DestroyWindow(w); } };
    struct SDLRendererDestroyer { void operator()(SDL_Renderer* r) const { if(r) SDL_DestroyRenderer(r); } };
    struct SDLTextureDestroyer { void operator()(SDL_Texture* t) const { if(t) SDL_DestroyTexture(t); } };
    struct TTFFontDestroyer { void operator()(TTF_Font* f) const { if(f) TTF_CloseFont(f); } };

    class Renderer {
    public:
        explicit Renderer(SDL_Window* window, const std::string& fontPath, int fontSize);
        ~Renderer() = default;

        Renderer(const Renderer&) = delete;
        Renderer& operator=(const Renderer&) = delete;
        Renderer(Renderer&&) noexcept = default;
        Renderer& operator=(Renderer&&) noexcept = default;

        [[nodiscard]] SDL_Renderer* getSDLRenderer() const;

        void clear() const;
        void present() const;

        void renderUI(int score, int highScore, int scoreX, int scoreY, int hsX, int hsY, SDL_Color textColor);

        void renderCenteredText(const std::string& text, int screenWidth, int screenHeight, SDL_Color color) const;

        [[nodiscard]] SDL_Point getTextSize(const std::string& text) const;

    private:
        std::unique_ptr<SDL_Renderer, SDLRendererDestroyer> sdlRenderer;
        std::unique_ptr<TTF_Font, TTFFontDestroyer> font;

        std::unique_ptr<SDL_Texture, SDLTextureDestroyer> scoreTexture;
        SDL_Rect scoreRect = {0, 0, 0, 0};
        int cachedScore = -1;
        SDL_Color scoreColor = {0,0,0,0};

        std::unique_ptr<SDL_Texture, SDLTextureDestroyer> highScoreTexture;
        SDL_Rect highScoreRect = {0, 0, 0, 0};
        int cachedHighScore = -1;
        SDL_Color highScoreColor = {0,0,0,0};

        [[nodiscard]] SDL_Texture* createTextTexture(const std::string& text, SDL_Color color) const;
    };

    class RendererError : public std::runtime_error {
    public:
        explicit RendererError(const std::string& message) : std::runtime_error(message) {}
    };

}

#endif