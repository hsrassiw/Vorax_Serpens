#ifndef FOOD_HPP
#define FOOD_HPP

#include <SDL.h>
#include <vector>
#include <random>
#include <deque>

namespace SnakeGame {

    /**
     *   Food
     *   Quản lý đối tượng thức ăn trong trò chơi Snake. Hiện tại chỉ có một loại thức ăn.
     *        Cần mở rộng nếu muốn có nhiều loại thức ăn với hiệu ứng khác nhau.
     */
    class Food {
    public:
        /**
         *   Khởi tạo đối tượng Food.
         *   cellSize Kích thước của mỗi ô (cell) trong game.
         */
        explicit Food(int cellSize);

        /**
         *   Tạo vị trí mới cho thức ăn, đảm bảo không trùng với thân rắn hoặc chướng ngại vật.
         *   screenWidth Chiều rộng màn hình (pixels).
         *   screenHeight Chiều cao màn hình (pixels).
         *   snakeBody Deque chứa các điểm (SDL_Point) của thân rắn.
         *   obstacles Vector chứa các điểm (SDL_Point) của chướng ngại vật (vị trí hiện tại).
         */
        void generate(int screenWidth, int screenHeight, const std::deque<SDL_Point>& snakeBody, const std::vector<SDL_Point>& obstacles);

        /**
         *   Lấy vị trí hiện tại của thức ăn.
         *   SDL_Point chứa tọa độ (x, y) của thức ăn. Trả về {-cellSize, -cellSize} nếu chưa được đặt hoặc không thể đặt.
         */
        [[nodiscard]] SDL_Point getPosition() const;

        /**
         *   Đặt thức ăn đến một vị trí cụ thể (hữu ích cho debug hoặc kịch bản).
         *   x Tọa độ x mới.
         *   y Tọa độ y mới.
         */
        void forcePosition(int x, int y);
    private:
        SDL_Point position; // Vị trí hiện tại của thức ăn
        int cellSize;       // Kích thước ô
        std::mt19937 rng;   // Bộ sinh số ngẫu nhiên Mersenne Twister
    };

}

#endif