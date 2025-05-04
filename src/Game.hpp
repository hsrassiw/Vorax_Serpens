#ifndef GAME_HPP
#define GAME_HPP

#include "Snake.hpp"
#include "Food.hpp"
#include "Renderer.hpp"
#include "Config.hpp"
#include <SDL.h>
#include <SDL_mixer.h>
#include <string>
#include <vector>
#include <memory>
#include <sstream>
#include <random>
#include <deque>
#include <algorithm>

namespace SnakeGame {

    /**
     *  GameMode
     *    Xác định các chế độ chơi khác nhau (ảnh hưởng đến va chạm tường).
     */
    enum class GameMode { Classic, PortalWalls };

    /**
     * GameState
     *    Xác định các trạng thái khác nhau của vòng lặp trò chơi.
     */
    enum class GameState { MainMenu, Options, Playing, Paused, GameOver, EnteringHighScore };

    /**
     *  MixChunkDeleter
     *    Functor để giải phóng Mix_Chunk, dùng với unique_ptr.
     */
    struct MixChunkDeleter {
        void operator()(Mix_Chunk* chunk) const {
            if (chunk) Mix_FreeChunk(chunk);
        }
    };

    /**
     *    HighScoreEntry
     *    Lưu trữ một mục điểm cao gồm tên và điểm.
     */
    struct HighScoreEntry {
        std::string name = "Player";
        int score = 0;

        /**
         *    Toán tử so sánh để sort (điểm cao hơn đứng trước).
         *    other Mục điểm cao khác để so sánh.
         *    true nếu điểm của mục này cao hơn điểm của mục other.
         */
        bool operator<(const HighScoreEntry& other) const {
            return score > other.score; // Sắp xếp giảm dần theo điểm
        }
    };

    /**
     *    MenuButton
     *    Đại diện cho một nút trên Main Menu (thường là vùng chữ nhật tương ứng trên ảnh nền).
     */
    struct MenuButton {
        SDL_Rect screenRect;   // Vị trí và kích thước trên màn hình
        GameState targetState; // Trạng thái sẽ chuyển đến khi nút được nhấn
        bool requestsQuit = false; // true nếu nút này dùng để thoát game
        std::string debugText;   // Tên nút để debug hoặc hiển thị nếu không có ảnh
    };

    /**
     *    OptionAction
     *    Xác định hành động sẽ thực hiện khi một mục trong menu Options được chọn.
     */
    enum class OptionAction {
        TOGGLE_MODE,       // Chuyển đổi giữa Classic/Portal
        TOGGLE_SOUND,      // Bật/tắt âm thanh
        RESET_HIGHSCORES, // Xóa điểm cao đã lưu
        GOTO_MAINMENU      // Quay lại menu chính
    };

    /**
     *    OptionItem
     *    Đại diện cho một mục trong menu Options.
     */
    struct OptionItem {
        std::string text;   // Text hiển thị cho mục
        OptionAction action; // Hành động tương ứng
    };

    /**
     *    ObstacleMovement
     *    Xác định kiểu di chuyển của chướng ngại vật.
     */
    enum class ObstacleMovement {
        Static,     // Đứng yên
        Horizontal, // Di chuyển ngang
        Vertical    // Di chuyển dọc
    };

    /**
     *    Obstacle
     *    Đại diện cho một chướng ngại vật (có thể tĩnh hoặc động).
     */
    struct Obstacle {
        SDL_Point position;        // Vị trí hiện tại (góc trên trái)
        ObstacleMovement movementType = ObstacleMovement::Static; // Kiểu di chuyển
        Direction moveDirection = Direction::RIGHT; // Hướng di chuyển hiện tại (cho động)
        int moveRange = 0;         // Số ô di chuyển tối đa theo một hướng trước khi đổi chiều (cho động)
        int currentMoveStep = 0; // Số bước đã di chuyển theo hướng hiện tại (cho động)
        int moveDelayCounter = 0; // Bộ đếm để làm chậm tốc độ di chuyển so với rắn
        int moveSpeedFactor = 3;  // Vật cản di chuyển sau mỗi X lượt rắn di chuyển (giá trị mặc định)
    };


    /**
     *    Game
     *    Lớp chính quản lý toàn bộ logic và luồng của trò chơi Snake.
     */
    class Game {
    public:
        /**
         *    Khởi tạo đối tượng Game.
         *    screenWidth Chiều rộng màn hình.
         *    screenHeight Chiều cao màn hình.
         *    cellSize Kích thước mỗi ô (cell).
         *    renderer Tham chiếu đến đối tượng Renderer để tải tài nguyên và vẽ.
         */
        Game(int screenWidth, int screenHeight, int cellSize, Renderer& renderer);
        /**    Destructor mặc định, unique_ptr sẽ tự dọn dẹp tài nguyên. */
        ~Game() = default;

        // Xóa các constructor/operator copy và move để tránh lỗi quản lý tài nguyên
        Game(const Game&) = delete;
        Game& operator=(const Game&) = delete;
        Game(Game&&) noexcept = default;
        Game& operator=(Game&&) noexcept = default;

        /**
         *    Xử lý đầu vào từ người dùng (bàn phím, chuột, thoát game).
         *    event Sự kiện SDL cần xử lý.
         */
        void handleInput(const SDL_Event& event);

        /**
         *    Cập nhật trạng thái trò chơi cho một bước logic (di chuyển rắn, kiểm tra va chạm, ăn mồi).
         *        Chỉ hoạt động khi ở trạng thái Playing.
         */
        void update();

        /**
         *    Vẽ trạng thái hiện tại của trò chơi lên màn hình dựa trên GameState.
         *    renderer Tham chiếu đến đối tượng Renderer để vẽ.
         * @note Hàm này không còn là const vì renderOptions() cần cập nhật optionsMenuItemRects.
         */
        void render(Renderer& renderer);

        /**
         *    Chạy một khung hình logic của trò chơi, bao gồm xử lý input, cập nhật và render.
         *        Quản lý tốc độ game dựa trên deltaTime và moveInterval/boost.
         *    deltaTime Thời gian trôi qua kể từ khung hình trước (tính bằng giây).
         */
        void runFrame(float deltaTime);

        /**
         *    Lấy trạng thái hiện tại của trò chơi.
         *    GameState Trạng thái hiện tại.
         */
        [[nodiscard]] GameState getCurrentState() const;

        /**
         *    Lấy điểm số hiện tại của người chơi.
         *    int Điểm số.
         */
        [[nodiscard]] int getScore() const { return score; }

        /**
         *    Kiểm tra xem người dùng có yêu cầu thoát trò chơi không.
         *    true nếu có yêu cầu thoát, false nếu không.
         */
        [[nodiscard]] bool didQuit() const;

        /**
         *    Đặt lại trạng thái trò chơi về ban đầu để bắt đầu một lượt chơi mới.
         *        Reset rắn, điểm, tốc độ, vật cản, thức ăn, trạng thái boost,...
         */
        void reset();

    private:
        // Kích thước màn hình và lưới
        int screenWidth;
        int screenHeight;
        int cellSize;

        // Đối tượng game chính
        Snake snake;
        Food food;
        std::vector<Obstacle> obstacles; // Danh sách các vật cản

        // Trạng thái game
        GameState currentState;
        GameMode currentGameMode;
        bool soundEnabled;
        int score;
        std::vector<HighScoreEntry> highScores;
        const int maxHighScores = Config::MAX_HIGH_SCORES;
        Uint32 moveInterval; // Khoảng thời gian giữa các bước (ms) - tốc độ nền
        bool quitRequested = false;
        float timeAccumulator = 0.0f; // Tích lũy thời gian cho game loop

        // Trạng thái Boost (Shift)
        bool isBoosting = false;      // Cờ cho biết có đang boost không
        float boostCostTimer = 0.0f;  // Bộ đếm thời gian (s) để áp dụng chi phí boost
        int boostCostCycles = 0;      // Đếm số lần trừ điểm để biết khi nào trừ chiều dài

        // Trạng thái UI và nhập liệu
        std::string currentPlayerNameInput; // Chuỗi tên đang nhập
        bool isEnteringName = false;      // Cờ cho biết có đang trong màn hình nhập tên không

        // Tài nguyên đồ họa và âm thanh (quản lý bằng unique_ptr)
        std::unique_ptr<SDL_Texture, SDLTextureDestroyer> menuTexture;
        std::unique_ptr<SDL_Texture, SDLTextureDestroyer> backgroundTexture;
        std::unique_ptr<SDL_Texture, SDLTextureDestroyer> foodTexture;
        // Thêm unique_ptr cho các texture táo khác nếu cần
        std::unique_ptr<Mix_Chunk, MixChunkDeleter> eatSound;
        std::unique_ptr<Mix_Chunk, MixChunkDeleter> collisionSound;
        std::unique_ptr<Mix_Chunk, MixChunkDeleter> gameOverSound;


        // Cache cho text thường dùng
        std::unique_ptr<SDL_Texture, SDLTextureDestroyer> pausedTextTexture;
        SDL_Rect pausedTextRect = {0,0,0,0};
        std::unique_ptr<SDL_Texture, SDLTextureDestroyer> gameOverTextTexture;
        SDL_Rect gameOverTextRect = {0,0,0,0};

        // UI Menu
        std::vector<MenuButton> menuButtons;      // Các nút ở Main Menu
        int selectedButtonIndex = 0;              // Chỉ số nút đang được chọn ở Main Menu
        std::vector<OptionItem> optionsMenuItems; // Các mục trong Options Menu
        int selectedOptionIndex = 0;              // Chỉ số mục đang được chọn ở Options Menu
        std::vector<SDL_Rect> optionsMenuItemRects; // Vùng chữ nhật bao quanh các mục Options (dùng cho click chuột)

        // Logic game khác
        int nextObstacleScoreThreshold; // Ngưỡng điểm để thêm vật cản mới
        std::mt19937 rng;               // Bộ sinh số ngẫu nhiên

        /**    Tải các tài nguyên (textures, sounds) và khởi tạo cache, nút menu. */
        void initAssets(Renderer& renderer);
        /**    Khởi tạo các mục trong menu tùy chọn và vector rect tương ứng. */
        void initOptions();
        /**    Cập nhật nội dung text của các mục options dựa trên trạng thái game (Mode, Sound). */
        void updateOptionTexts();
        /**    Tính toán vị trí xuất phát ban đầu cho rắn (thường là giữa màn hình). */
        [[nodiscard]] SDL_Point calculateStartPosition() const;
        /**    Tăng tốc độ di chuyển nền của rắn (giảm moveInterval) sau khi ăn mồi. */
        void increaseSpeed();
        /**    Đặt thức ăn vào một vị trí mới hợp lệ trên lưới, tránh rắn và vật cản. */
        void placeFood();
        /**    Tạo các chướng ngại vật ban đầu (tĩnh và động) khi bắt đầu game. */
        void generateObstacles();
        /**    Kiểm tra va chạm giữa một điểm (thường là đầu rắn) và vị trí hiện tại của các chướng ngại vật. */
        [[nodiscard]] bool checkObstacleCollision(const SDL_Point& pos) const;
        /**    Thêm một chướng ngại vật mới (thường là động) vào vị trí ngẫu nhiên hợp lệ khi đạt ngưỡng điểm. */
        bool addSingleObstacle();
        /**    Cập nhật vị trí của các chướng ngại vật động, xử lý va chạm của chúng và đổi hướng nếu cần. */
        void updateObstacles();

        /**    Tải điểm cao từ tệp vào vector highScores. */
        void loadHighScores();
        /**    Lưu danh sách điểm cao hiện tại (trong vector highScores) vào tệp. */
        void saveHighScores() const;
        /**    Kiểm tra xem điểm số hiện tại có đủ cao để vào bảng xếp hạng không. */
        [[nodiscard]] bool isHighScore(int currentScore) const;
        /**    Thêm một mục điểm cao mới vào danh sách, sắp xếp lại và giới hạn số lượng. */
        void addHighScore(const std::string& name, int score);

        /**    Xử lý sự kiện bàn phím/text input khi đang trong trạng thái nhập điểm cao. */
        void handleHighScoreInput(const SDL_Event& event);
        /**    Xử lý sự kiện nhấn phím ESC chung cho các trạng thái. */
        void handleEscapeKey();
        /**    Xử lý input (chuột, phím) cho trạng thái MainMenu. */
        void handleMainMenuInput(const SDL_Event& event, Config::ControlInput control);
        /**    Xử lý input (chuột, phím) cho trạng thái Options. */
        void handleOptionsInput(const SDL_Event& event, Config::ControlInput control);
        /**    Xử lý input (chỉ hướng và pause) cho trạng thái Playing. */
        void handlePlayingInput(const SDL_Event& event, Config::ControlInput control);
        /**    Xử lý input (resume) cho trạng thái Paused. */
        void handlePausedInput(const SDL_Event& event, Config::ControlInput control);
        /**    Xử lý input (restart) cho trạng thái GameOver. */
        void handleGameOverInput(const SDL_Event& event, Config::ControlInput control);

        /**    Kích hoạt hành động của nút được chọn trên Main Menu (chuyển state hoặc quit). */
        void activateMainMenuButton(int index);
        /**    Kích hoạt hành động của mục được chọn trong menu Options (thay đổi cài đặt hoặc quay lại). */
        void activateOptionItem(int index);
        /**    Chuyển đổi giữa các chế độ chơi Classic và PortalWalls. */
        void toggleGameMode();

        /**
         *    Xử lý logic bắt đầu/dừng boost dựa trên trạng thái phím Shift và điều kiện game.
         *        Đồng thời áp dụng chi phí (điểm, chiều dài) nếu đang boost.
         *    deltaTime Thời gian trôi qua của khung hình hiện tại, dùng để tính thời gian áp dụng cost.
         */
        void handleBoosting(float deltaTime);

        // Các hàm vẽ cho từng trạng thái
        /**    Vẽ màn hình Main Menu, bao gồm các nút và danh sách điểm cao. */
        void renderMainMenu(Renderer& renderer) const;
        /**    Vẽ màn hình Options, bao gồm các mục tùy chọn và cập nhật vùng rect của chúng. */
        void renderOptions(Renderer& renderer); // Không const vì cập nhật optionsMenuItemRects
        /**    Vẽ màn hình khi đang chơi, tạm dừng hoặc game over (vẽ rắn, mồi, vật cản, UI, thông báo). */
        void renderGameScreen(Renderer& renderer) const;
        /**    Vẽ màn hình nhập điểm cao với lớp phủ và ô nhập text. */
        void renderHighScoreEntry(Renderer& renderer) const;

    };

}

#endif