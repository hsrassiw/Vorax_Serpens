#ifndef VORAX_SERPENS_CONFIG_HPP
#define VORAX_SERPENS_CONFIG_HPP

#include <SDL.h>
#include <string>


namespace SnakeGame {
    namespace Config {

        // --- Cài đặt màn hình và lưới ---
        constexpr int SCREEN_WIDTH = 1000;
        constexpr int SCREEN_HEIGHT = 800;
        constexpr int CELL_SIZE = 20;

        // --- Cài đặt Rắn ---
        constexpr int DEFAULT_SNAKE_LENGTH = 3;
        constexpr int INITIAL_SNAKE_SPEED_DELAY_MS = 150; // Khoảng thời gian (ms) giữa các bước di chuyển ban đầu
        constexpr int SPEED_INCREMENT_MS = 4;             // Mức giảm delay mỗi khi ăn mồi
        constexpr int MIN_MOVE_INTERVAL_MS = 50;          // Khoảng thời gian tối thiểu (tốc độ tối đa thông thường)
        constexpr int SNAKE_INPUT_BUFFER_SIZE = 2;        // Kích thước bộ đệm lệnh di chuyển

        // --- Cài đặt Boost (Shift) ---
        constexpr int MIN_BOOST_LENGTH = 3;           // Chiều dài tối thiểu của rắn để có thể boost
        constexpr int BOOST_MOVE_INTERVAL_MS = 60;    // Khoảng thời gian (ms) giữa các bước khi boost (phải >= MIN_MOVE_INTERVAL_MS)
        constexpr int BOOST_SCORE_COST = 1;           // Số điểm bị trừ mỗi lần áp dụng chi phí boost
        constexpr int BOOST_COST_INTERVAL_MS = 200;   // Áp dụng chi phí boost mỗi 200ms
        constexpr int BOOST_LENGTH_COST_INTERVALS = 1;// Giảm chiều dài rắn sau mỗi X lần trừ điểm

        // --- Cài đặt Game ---
        constexpr int MAX_HIGH_SCORES = 5;                // Số lượng điểm cao tối đa hiển thị/lưu trữ
        constexpr int OBSTACLE_COUNT = 10;                // Số lượng vật cản ban đầu
        constexpr int OBSTACLE_ADD_SCORE_INTERVAL = 2;    // Thêm vật cản mới sau mỗi X điểm


        // --- Cấu hình Độ khó Vật cản theo Điểm ---
        constexpr float BASE_MOVING_OBSTACLE_RATIO = 0.3f;  // Tỷ lệ vật cản động ban đầu (30%)
        constexpr float MAX_MOVING_OBSTACLE_RATIO = 0.8f;   // Tỷ lệ vật cản động tối đa (80%)
        constexpr float MOVING_RATIO_SCORE_FACTOR = 0.005f; // Tỷ lệ tăng thêm cho mỗi điểm (0.5% / điểm)

        constexpr int BASE_OBSTACLE_SPEED_FACTOR = 3;   // Factor tốc độ ban đầu (chậm)
        constexpr int MIN_OBSTACLE_SPEED_FACTOR = 1;    // Factor tốc độ tối thiểu (nhanh nhất)
        constexpr int OBSTACLE_SPEED_SCORE_DIVISOR = 50; // Giảm speed factor sau mỗi X điểm

        // --- Màu sắc ---
        constexpr SDL_Color SNAKE_COLOR = {0, 255, 0, 255};     // Màu thân rắn
         constexpr SDL_Color SNAKE_HEAD_COLOR = {0, 200, 0, 255}; // Tùy chọn: màu đầu rắn khác
        constexpr SDL_Color OBSTACLE_COLOR = {100, 100, 100, 255}; // Màu vật cản
        constexpr SDL_Color BACKGROUND_COLOR = {30, 30, 30, 255}; // Màu nền mặc định
        constexpr SDL_Color TEXT_COLOR = {255, 255, 255, 255};  // Màu text chung
        constexpr SDL_Color GAMEOVER_TEXT_COLOR = {255, 50, 50, 255}; // Màu chữ Game Over
        constexpr SDL_Color PAUSE_TEXT_COLOR = {200, 200, 200, 255}; // Màu chữ Paused
        constexpr SDL_Color MENU_HIGHLIGHT_COLOR = {255, 255, 150, 255}; // Màu highlight nút menu chính
        constexpr SDL_Color OPTIONS_TEXT_COLOR = {220, 220, 220, 255}; // Màu chữ menu options
        constexpr SDL_Color OPTIONS_HIGHLIGHT_COLOR = {255, 255, 255, 255}; // Màu highlight mục options

        // --- Đường dẫn Tài nguyên ---
        const std::string FONT_PATH = "assets/fonts/Roboto-Regular.ttf"; // Đường dẫn font
        constexpr int FONT_SIZE = 24;                                    // Kích thước font hiển thị
        const std::string MENU_IMAGE_PATH = "assets/images/main_menu.png";   // Ảnh nền menu chính
        const std::string BACKGROUND_IMAGE_PATH = "assets/images/Space_Background.png"; // Ảnh nền trong game
        const std::string FOOD_IMAGE_PATH = "assets/images/apple.png";       // Ảnh mồi (mặc định)
        const std::string HIGHSCORE_FILE = "highscore.dat";                  // Tên file lưu điểm cao
        const std::string EAT_SOUND_PATH = "assets/sounds/eat.wav";        // Âm thanh ăn mồi
        const std::string COLLISION_SOUND_PATH = "assets/sounds/hit.wav"; // Âm thanh va chạm chung
        const std::string GAME_OVER_SOUND_PATH = "assets/sounds/game_over.wav"; // Âm thanh kết thúc

        /**
         * Xác định các hướng di chuyển có thể của rắn.
         */
        enum class Direction { UP, DOWN, LEFT, RIGHT };

        /**
         *  ControlInput
         *  Đại diện cho các hành động điều khiển của người chơi, được dịch từ input thô.
         */
        enum class ControlInput {
            UP, DOWN, LEFT, RIGHT,
            RESTART, // Space
            PAUSE,   // P
            CONFIRM, // Enter
            NONE     // Không có input liên quan hoặc ESC (xử lý riêng)
        };

        /**
         * event Sự kiện SDL cần xử lý.
         * ControlInput tương ứng, hoặc ControlInput::NONE nếu sự kiện không liên quan.
         */
        ControlInput handleRawInput(const SDL_Event &event);

    }
}
#endif