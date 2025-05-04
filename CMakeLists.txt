cmake_minimum_required(VERSION 3.30)
project(Vorax_Serpens)

set(CMAKE_CXX_STANDARD 20)

add_definitions(-DSDL_MAIN_HANDLED)


set(SDL2_BASE_DIR "${CMAKE_SOURCE_DIR}/libs") # Hoặc đường dẫn tuyệt đối nếu muốn

set(SDL2_DIR "${SDL2_BASE_DIR}/SDL2")
set(SDL2_IMAGE_DIR "${SDL2_BASE_DIR}/SDL2_image")
set(SDL2_MIXER_DIR "${SDL2_BASE_DIR}/SDL2_mixer")
set(SDL2_TTF_DIR "${SDL2_BASE_DIR}/SDL2_ttf")


# Tự động lấy tất cả file .cpp trong src/

set(SRC_FILES
        src/main.cpp
        src/Game.cpp
        src/Snake.cpp
        src/Food.cpp
        src/Renderer.cpp
        src/Config.cpp
)
# Header không cần liệt kê ở đây, CMake sẽ tự tìm dependency

# Tạo executable
add_executable(Vorax_Serpens ${SRC_FILES})


target_include_directories(Vorax_Serpens PRIVATE
        "${SDL2_DIR}/x86_64-w64-mingw32/include/SDL2"
        "${SDL2_IMAGE_DIR}/x86_64-w64-mingw32/include/SDL2"
        "${SDL2_MIXER_DIR}/x86_64-w64-mingw32/include/SDL2"
        "${SDL2_TTF_DIR}/x86_64-w64-mingw32/include/SDL2"
)


target_link_directories(Vorax_Serpens PRIVATE
        "${SDL2_DIR}/x86_64-w64-mingw32/lib"
        "${SDL2_IMAGE_DIR}/x86_64-w64-mingw32/lib"
        "${SDL2_MIXER_DIR}/x86_64-w64-mingw32/lib"
        "${SDL2_TTF_DIR}/x86_64-w64-mingw32/lib"
)

# Liên kết thư viện SDL2
target_link_libraries(Vorax_Serpens PRIVATE
        mingw32
        SDL2main
        SDL2 SDL2_image SDL2_mixer SDL2_ttf
)


set(ASSET_SOURCE_DIR ${CMAKE_SOURCE_DIR}/assets)
set(ASSET_DESTINATION_DIR ${CMAKE_BINARY_DIR}/assets)

# Lệnh copy đơn giản - chạy mỗi khi CMake cấu hình
file(COPY ${ASSET_SOURCE_DIR} DESTINATION ${CMAKE_BINARY_DIR})

