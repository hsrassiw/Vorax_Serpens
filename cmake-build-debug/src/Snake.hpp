#ifndef SNAKE_HPP
#define SNAKE_HPP

#include <deque>
#include <vector>
#include <SDL.h>
#include "Config.hpp"

namespace SnakeGame {


    using Config::Direction;

    /**
     *    Snake
     *    Quản lý trạng thái và hành vi của con rắn trong trò chơi.
     *        Bao gồm vị trí các đốt, hướng di chuyển, trạng thái phát triển,
     *        bộ đệm input và các phương thức liên quan.
     */
    class Snake {
    public:
        /**
         *    Khởi tạo đối tượng Snake.
         *    startX Tọa độ x ban đầu của đầu rắn (góc trên trái).
         *    startY Tọa độ y ban đầu của đầu rắn (góc trên trái).
         *    cellSize Kích thước của mỗi ô (đốt rắn).
         *    initialLength Chiều dài ban đầu của rắn (số đốt).
         */
        Snake(int startX, int startY, int cellSize, int initialLength);

        /**
         *    Thực hiện di chuyển rắn một bước. Thêm đốt mới vào đầu và
         *        xóa đốt đuôi nếu rắn không đang trong trạng thái phát triển ('growing').
         *    nextHead Vị trí đầu rắn tiếp theo đã được tính toán (thường từ calculateNextHeadPosition).
         */
        void move(const SDL_Point& nextHead);

        /**
         *    Vẽ con rắn lên màn hình bằng cách vẽ các hình chữ nhật cho từng đốt.
         *    renderer Con trỏ tới SDL_Renderer để vẽ.
         */
        void draw(SDL_Renderer* renderer) const;

        /**
         *    Thêm một yêu cầu thay đổi hướng vào bộ đệm đầu vào.
         *        Bộ đệm giúp xử lý các lần nhấn phím nhanh, tránh bỏ lỡ input.
         *        Hướng mới chỉ được thêm nếu hợp lệ (không đầy buffer, không ngược hướng hiệu quả cuối cùng).
         *    newDirection Hướng mới yêu cầu.
         */
        void queueDirectionChange(Direction newDirection);

        /**
         *    Đánh dấu rằng rắn sẽ phát triển thêm một đốt trong lần di chuyển ('move') tiếp theo.
         *        Thường được gọi sau khi ăn mồi. Cờ 'growing' sẽ được reset trong hàm move().
         */
        void grow();

        /**
         *    Giảm chiều dài của rắn đi một đốt bằng cách xóa đốt cuối cùng (đuôi).
         *        Thường được gọi khi sử dụng tính năng boost và hết chu kỳ cost.
         *        Không làm gì nếu rắn chỉ còn 1 đốt.
         */
        void shrink();

        /**
         *    Tính toán vị trí tiềm năng tiếp theo của đầu rắn dựa trên hướng di chuyển hiệu quả hiện tại.
         *        Hàm này sẽ gọi processAndApplyInputBuffer() để cập nhật hướng trước khi tính toán.
         *    SDL_Point Vị trí (góc trên trái) dự kiến của đầu rắn trong bước di chuyển tiếp theo.
         *         Trả về {-1, -1} nếu rắn không có thân (trường hợp lỗi).
         */
        [[nodiscard]] SDL_Point calculateNextHeadPosition();

        /**
         *    Kiểm tra xem đầu rắn (vị trí hiện tại) có va chạm với vị trí thức ăn không.
         *    foodPos Vị trí của thức ăn cần kiểm tra.
         *    true nếu đầu rắn ở cùng vị trí với thức ăn, false nếu không.
         */
        [[nodiscard]] bool checkFoodCollision(const SDL_Point& foodPos) const;

        /**
         *    Kiểm tra xem đầu rắn (vị trí hiện tại) có va chạm với bất kỳ phần nào của thân nó không.
         *        (Ít dùng trong logic chính, thường dùng checkSelfCollisionWithNext để dự đoán).
         *    true nếu có va chạm với thân, false nếu không hoặc rắn quá ngắn.
         */
        [[nodiscard]] bool checkSelfCollision() const;

        /**
         *    Kiểm tra xem vị trí đầu rắn *tiềm năng* tiếp theo có va chạm với thân rắn hiện tại không.
         *        Đây là hàm kiểm tra va chạm bản thân chính được sử dụng trong logic game trước khi di chuyển.
         *        Sẽ bỏ qua kiểm tra với đốt đuôi nếu rắn không đang phát triển ('growing' = false).
         *    nextHead Vị trí đầu rắn tiềm năng trong bước di chuyển tới.
         *    true nếu vị trí tiềm năng va chạm với thân (trừ đuôi nếu không growing), false nếu không.
         */
        [[nodiscard]] bool checkSelfCollisionWithNext(const SDL_Point& nextHead) const;

        /**
         *    Lấy tham chiếu không đổi tới deque chứa các đốt của thân rắn.
         *        Hữu ích cho việc kiểm tra va chạm hoặc lấy thông tin chiều dài.
         *    const std::deque<SDL_Point>& Tham chiếu không đổi tới deque thân rắn.
         */
        [[nodiscard]] const std::deque<SDL_Point>& getBody() const;

        /**
         *    Lấy vị trí hiện tại của đầu rắn (đốt đầu tiên).
         *    SDL_Point Vị trí đầu rắn, hoặc {-1, -1} nếu rắn không có thân.
         */
        [[nodiscard]] SDL_Point getHeadPosition() const;

        /**
         *    Lấy kích thước của mỗi ô (đốt rắn).
         *    int Kích thước ô (thường là Config::CELL_SIZE).
         */
        [[nodiscard]] int getCellSize() const { return cellSize; }

        /**
         *    Lấy hướng di chuyển hiện tại của rắn (hướng đã được xác nhận và sử dụng trong bước di chuyển trước).
         *    Direction Hướng hiện tại.
         */
        [[nodiscard]] Direction getCurrentDirection() const;

        /**
        *    Xử lý một lệnh trong bộ đệm đầu vào (nếu có) và cập nhật hướng di chuyển hiện tại ('currentDirection').
        *        Được gọi bên trong calculateNextHeadPosition().
        */
        void processAndApplyInputBuffer();


    private:
        std::deque<SDL_Point> body;          // Deque lưu trữ vị trí các đốt rắn (đầu ở front)
        Direction currentDirection;          // Hướng di chuyển hiện tại đã xác nhận
        std::vector<Direction> inputBuffer; // Hàng đợi các lệnh đổi hướng từ người chơi
        bool growing = false;                // Cờ cho biết rắn có đang lớn lên không
        int cellSize;                        // Kích thước của một đốt rắn

        /**
         *    Kiểm tra xem hai hướng có đối nghịch nhau không (ví dụ: UP và DOWN).
         *    dir1 Hướng thứ nhất.
         *    dir2 Hướng thứ hai.
         *    true nếu hai hướng đối nghịch, false nếu không.
         */
        [[nodiscard]] bool isOppositeDirection(Direction dir1, Direction dir2) const;
    };

}

#endif