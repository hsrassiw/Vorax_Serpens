#ifndef RENDERER_HPP
#define RENDERER_HPP

#include <SDL.h>
#include <SDL_ttf.h>
#include <string>
#include <stdexcept>
#include <memory>
#include <vector>
#include <cstring>

namespace SnakeGame {

    /**    Functor để hủy SDL_Window, dùng với std::unique_ptr. */
    struct SDLWindowDestroyer { void operator()(SDL_Window* w) const { if(w) SDL_DestroyWindow(w); } };
    /**    Functor để hủy SDL_Renderer, dùng với std::unique_ptr. */
    struct SDLRendererDestroyer { void operator()(SDL_Renderer* r) const { if(r) SDL_DestroyRenderer(r); } };
    /**    Functor để hủy SDL_Texture, dùng với std::unique_ptr. */
    struct SDLTextureDestroyer { void operator()(SDL_Texture* t) const { if(t) SDL_DestroyTexture(t); } };
    /**    Functor để hủy TTF_Font, dùng với std::unique_ptr. */
    struct TTFFontDestroyer { void operator()(TTF_Font* f) const { if(f) TTF_CloseFont(f); } };

    /**
     *    RendererError
     *    Lớp ngoại lệ tùy chỉnh cho các lỗi liên quan đến Renderer (ví dụ: không tạo được renderer, không load được font).
     */
    class RendererError : public std::runtime_error {
    public:
        explicit RendererError(const std::string& message) : std::runtime_error(message) {}
    };

    /**
     *    Renderer
     *    Đóng gói các hoạt động vẽ và quản lý tài nguyên đồ họa (textures, font) bằng SDL.
     *        Sử dụng kỹ thuật supersampling cho font để chữ mịn hơn.
     */
    class Renderer {
    public:
        /**
         *    Khởi tạo Renderer với cửa sổ và font được chỉ định.
         *    window Con trỏ tới SDL_Window mà Renderer sẽ vẽ lên.
         *    fontPath Đường dẫn tới tệp font TTF.
         *    fontSize Kích thước font mong muốn hiển thị trên màn hình (sẽ được render lớn hơn và thu nhỏ).
         * @throws RendererError Nếu không thể tạo SDL_Renderer hoặc tải font.
         */
        explicit Renderer(SDL_Window* window, const std::string& fontPath, int fontSize);
        /**    Destructor mặc định, unique_ptr sẽ tự giải phóng tài nguyên SDL. */
        ~Renderer() = default;

        // Xóa các constructor/operator copy và move
        Renderer(const Renderer&) = delete;
        Renderer& operator=(const Renderer&) = delete;
        Renderer(Renderer&&) noexcept = default;
        Renderer& operator=(Renderer&&) noexcept = default;

        /**
         *    Lấy con trỏ tới đối tượng SDL_Renderer gốc.
         *    SDL_Renderer* Con trỏ tới SDL_Renderer, hoặc nullptr nếu chưa khởi tạo.
         */
        [[nodiscard]] SDL_Renderer* getSDLRenderer() const;

        /**
         *    Tải một texture từ tệp ảnh sử dụng SDL_image.
         *    path Đường dẫn tới tệp ảnh (PNG, JPG,...).
         *    SDL_Texture* Con trỏ tới texture đã tải. Caller chịu trách nhiệm quản lý bộ nhớ (thường dùng unique_ptr). Trả về nullptr nếu lỗi.
         */
        [[nodiscard]] SDL_Texture* loadTexture(const std::string& path) const;

        /**
         *    Xóa toàn bộ màn hình vẽ hiện tại bằng màu nền mặc định (Config::BACKGROUND_COLOR).
         */
        void clear() const;

        /**
         *    Hiển thị nội dung đã được vẽ trong bộ đệm lên cửa sổ.
         */
        void present() const;

        /**
         *    Vẽ toàn bộ một texture lên một vị trí và kích thước xác định trên màn hình.
         *    texture Con trỏ tới SDL_Texture cần vẽ.
         *    destRect Con trỏ tới SDL_Rect xác định khu vực đích để vẽ.
         */
        void drawTexture(SDL_Texture* texture, const SDL_Rect* destRect) const;

        /**
         *    Vẽ một phần của texture nguồn lên một vị trí và kích thước xác định trên màn hình.
         *    texture Con trỏ tới SDL_Texture nguồn.
         *    srcRect Con trỏ tới SDL_Rect xác định phần của texture nguồn sẽ được lấy.
         *    destRect Con trỏ tới SDL_Rect xác định khu vực đích để vẽ.
         */
        void drawTexturePortion(SDL_Texture* texture, const SDL_Rect* srcRect, const SDL_Rect* destRect) const;

        /**
         *    Vẽ một hình chữ nhật đơn lẻ.
         *    rect Con trỏ tới SDL_Rect để vẽ.
         *    color Màu sắc của hình chữ nhật (SDL_Color).
         *    filled True nếu muốn vẽ hình đặc, False nếu chỉ vẽ đường viền.
         */
        void drawRect(const SDL_Rect* rect, SDL_Color color, bool filled = false) const;

        /**
         *    Vẽ một danh sách các hình chữ nhật cùng lúc (hiệu quả hơn vẽ từng cái nếu cùng màu).
         *    rects Vector chứa các SDL_Rect cần vẽ.
         *    color Màu sắc chung cho tất cả hình chữ nhật.
         *    filled True nếu muốn vẽ hình đặc, False nếu chỉ vẽ đường viền.
         */
        void drawRects(const std::vector<SDL_Rect>& rects, SDL_Color color, bool filled = false) const;

        /**
         *    Tạo một SDL_Texture từ text, sử dụng font đã scale và blending.
         *        Texture tạo ra sẽ có kích thước lớn hơn kích thước hiển thị mong muốn (do FONT_RENDER_SCALE).
         *    text Chuỗi văn bản UTF-8 cần render.
         *    color Màu chữ mong muốn.
         *    SDL_Texture* Con trỏ tới texture mới được tạo. Caller quản lý bộ nhớ (dùng unique_ptr). Trả về nullptr nếu lỗi.
         */
        [[nodiscard]] SDL_Texture* createTextTexture(const std::string& text, SDL_Color color) const;

        /**
         *    Render các yếu tố UI cơ bản (điểm số, điểm cao) với cơ chế cache để tối ưu hiệu suất.
         *        Hàm này không const vì nó cập nhật các thành viên cache (texture, rect, màu, giá trị).
         *    score Điểm số hiện tại.
         *    highScore Điểm cao nhất hiện tại.
         *    scoreX Tọa độ X góc trên bên trái để vẽ Score.
         *    scoreY Tọa độ Y góc trên bên trái để vẽ Score.
         *    hsX Tọa độ X góc trên bên trái để vẽ High Score.
         *    hsY Tọa độ Y góc trên bên trái để vẽ High Score.
         *    textColor Màu chữ cho cả hai.
         */
        void renderUI(int score, int highScore, int scoreX, int scoreY, int hsX, int hsY, SDL_Color textColor);

        /**
         *    Render văn bản căn giữa theo chiều ngang tại một vị trí Y cụ thể, sử dụng supersampling.
         *        Hàm này không const vì dùng cache texture tạm thời (textTextureCache).
         *    text Chuỗi văn bản cần vẽ.
         *    screenWidth Chiều rộng màn hình để căn giữa.
         *    yPos Tọa độ Y mong muốn (cho đỉnh của text).
         *    color Màu chữ.
         */
        void renderCenteredText(const std::string& text, int screenWidth, int yPos, SDL_Color color);

        /**
        *    Render văn bản tại một vị trí X, Y cụ thể, sử dụng supersampling.
        *        Hàm này không const vì dùng cache texture tạm thời (textTextureCache).
        *    text Chuỗi văn bản cần render.
        *    x Tọa độ X góc trên bên trái.
        *    y Tọa độ Y góc trên bên trái.
        *    color Màu chữ.
        */
        void renderText(const std::string& text, int x, int y, SDL_Color color);

        /**
         *    Lấy kích thước (rộng, cao) mà văn bản sẽ chiếm khi render với font hiện tại và áp dụng downscale.
         *        Quan trọng cho việc tính toán layout chính xác trên màn hình.
         *    text Chuỗi văn bản cần đo.
         *    SDL_Point Chứa width (x) và height (y) dự kiến trên màn hình. Trả về {0, 0} nếu lỗi.
         */
        [[nodiscard]] SDL_Point getTextSize(const std::string& text) const;

        /**
         *    Truy vấn thông tin của một texture (kích thước thực tế, định dạng, kiểu truy cập).
         *    texture Texture cần truy vấn.
         *    format (Tùy chọn) Con trỏ nhận định dạng pixel (có thể là nullptr).
         *    access (Tùy chọn) Con trỏ nhận kiểu truy cập (có thể là nullptr).
         *    w (Bắt buộc) Con trỏ nhận chiều rộng thực tế của texture.
         *    h (Bắt buộc) Con trỏ nhận chiều cao thực tế của texture.
         *    0 nếu thành công, < 0 nếu lỗi (ví dụ: texture là nullptr).
         */
        int queryTexture(SDL_Texture* texture, Uint32* format, int* access, int* w, int* h) const;


    private:
        std::unique_ptr<SDL_Renderer, SDLRendererDestroyer> sdlRenderer; // Con trỏ tới SDL Renderer
        std::unique_ptr<TTF_Font, TTFFontDestroyer> font;                // Con trỏ tới font đã load (ở kích thước lớn)

        // Cache cho UI động (Score, High Score) để tránh tạo texture mỗi frame
        std::unique_ptr<SDL_Texture, SDLTextureDestroyer> scoreTexture;
        SDL_Rect scoreRect = {0, 0, 0, 0};       // Rect đích để vẽ score
        int cachedScore = -1;                    // Giá trị score đã cache
        SDL_Color scoreColor = {0,0,0,0};        // Màu score đã cache

        std::unique_ptr<SDL_Texture, SDLTextureDestroyer> highScoreTexture;
        SDL_Rect highScoreRect = {0, 0, 0, 0};   // Rect đích để vẽ high score
        int cachedHighScore = -1;                // Giá trị high score đã cache
        SDL_Color highScoreColor = {0,0,0,0};    // Màu high score đã cache

        // Cache cho renderText và renderCenteredText (dùng chung, sẽ bị ghi đè thường xuyên)
        std::unique_ptr<SDL_Texture, SDLTextureDestroyer> textTextureCache;
        SDL_Rect textRectCache = {0,0,0,0};      // Rect đích cho text thường
    };


}

#endif