#include "Snake.hpp"
#include "Config.hpp"
#include <vector>
#include <algorithm>

namespace SnakeGame {

    Snake::Snake(int startX, int startY, int size, int initialLength)
            : currentDirection(Direction::RIGHT),
              growing(false),
              cellSize(size)
    {
        if (initialLength < 1) initialLength = 1;
        for (int i = 0; i < initialLength; ++i) {
            body.push_back({startX - i * cellSize, startY});
        }
        inputBuffer.reserve(Config::SNAKE_INPUT_BUFFER_SIZE);
    }

    bool Snake::isOppositeDirection(Direction dir1, Direction dir2) const {
        return (dir1 == Direction::UP && dir2 == Direction::DOWN) ||
               (dir1 == Direction::DOWN && dir2 == Direction::UP) ||
               (dir1 == Direction::LEFT && dir2 == Direction::RIGHT) ||
               (dir1 == Direction::RIGHT && dir2 == Direction::LEFT);
    }

    void Snake::processAndApplyInputBuffer() {
        if (!inputBuffer.empty()) {
            Direction nextDir = inputBuffer.front();
            inputBuffer.erase(inputBuffer.begin());
            if (!isOppositeDirection(currentDirection, nextDir)) {
                currentDirection = nextDir;
            }
        }
    }

    void Snake::move(const SDL_Point& nextHead) {
        if (body.empty()) return;
        body.push_front(nextHead);
        if (growing) {
            growing = false;
        } else {
            if (!body.empty()) {
                body.pop_back();
            }
        }
    }

    void Snake::draw(SDL_Renderer* renderer) const {
        if (!renderer || body.empty()) return;
        const SDL_Point& headPos = body.front();
        SDL_Rect headRect = { headPos.x, headPos.y, cellSize, cellSize }; // Tạo hình chữ nhật cho đầu
        SDL_SetRenderDrawColor(renderer, Config::SNAKE_HEAD_COLOR.r, Config::SNAKE_HEAD_COLOR.g, Config::SNAKE_HEAD_COLOR.b, Config::SNAKE_HEAD_COLOR.a);
        // Vẽ hình chữ nhật đặc cho đầu rắn
        SDL_RenderFillRect(renderer, &headRect);
        if (body.size() > 1) {
            std::vector<SDL_Rect> bodyRects;
            bodyRects.reserve(body.size() - 1);
            for (size_t i = 1; i < body.size(); ++i) {
                const SDL_Point& segmentPos = body[i];
                bodyRects.push_back({ segmentPos.x, segmentPos.y, cellSize, cellSize });
            }

            SDL_SetRenderDrawColor(renderer, Config::SNAKE_COLOR.r, Config::SNAKE_COLOR.g, Config::SNAKE_COLOR.b, Config::SNAKE_COLOR.a);
            SDL_RenderFillRects(renderer, bodyRects.data(), static_cast<int>(bodyRects.size()));
        }
    }

    void Snake::queueDirectionChange(Direction newDirection) {
        Direction lastEffectiveDirection = inputBuffer.empty() ? currentDirection : inputBuffer.back();
        if (inputBuffer.size() < Config::SNAKE_INPUT_BUFFER_SIZE &&
            !isOppositeDirection(lastEffectiveDirection, newDirection) &&
            newDirection != lastEffectiveDirection)
        {
            inputBuffer.push_back(newDirection);
        }
    }

    void Snake::grow() {
        growing = true;
    }

    void Snake::shrink() {
        if (body.size() > 1) {
            body.pop_back();
        }
    }

    SDL_Point Snake::calculateNextHeadPosition() {
        if (body.empty()) return {-1, -1};
        processAndApplyInputBuffer();
        SDL_Point nextHead = body.front();
        switch (currentDirection) {
            case Direction::UP:    nextHead.y -= cellSize; break;
            case Direction::DOWN:  nextHead.y += cellSize; break;
            case Direction::LEFT:  nextHead.x -= cellSize; break;
            case Direction::RIGHT: nextHead.x += cellSize; break;
        }
        return nextHead;
    }

    bool Snake::checkFoodCollision(const SDL_Point& foodPos) const {
        if (body.empty()) return false;
        return body.front().x == foodPos.x && body.front().y == foodPos.y;
    }

    bool Snake::checkSelfCollision() const {
        if (body.size() < 2) return false;
        const SDL_Point& head = body.front();
        for (size_t i = 1; i < body.size(); ++i) {
            if (head.x == body[i].x && head.y == body[i].y) {
                return true;
            }
        }
        return false;
    }

    bool Snake::checkSelfCollisionWithNext(const SDL_Point& nextHead) const {
        if (body.empty()) return false;
        size_t checkLimit = body.size();
        if (!growing && body.size() > 1) {
            checkLimit = body.size() - 1;
        }
        for (size_t i = 0; i < checkLimit; ++i) {
            if (nextHead.x == body[i].x && nextHead.y == body[i].y) {
                return true;
            }
        }
        return false;
    }

    const std::deque<SDL_Point>& Snake::getBody() const {
        return body;
    }

    SDL_Point Snake::getHeadPosition() const {
        return body.empty() ? SDL_Point{-1, -1} : body.front();
    }

    Direction Snake::getCurrentDirection() const {
        return currentDirection;
    }

}
