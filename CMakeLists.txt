cmake_minimum_required(VERSION 3.30)
project(Vorax_Serpens)

set(CMAKE_CXX_STANDARD 20)

add_definitions(-DSDL_MAIN_HANDLED)

# ===============================
# Đường dẫn tuyệt đối tới thư viện SDL2
# ===============================
set(SDL2_DIR "D:/Clion/Vorax_Serpens/cmake-build-debug/SDL2")
set(SDL2_IMAGE_DIR "D:/Clion/Vorax_Serpens/cmake-build-debug/SDL2_image")
set(SDL2_MIXER_DIR "D:/Clion/Vorax_Serpens/cmake-build-debug/SDL2_mixer")
set(SDL2_TTF_DIR "D:/Clion/Vorax_Serpens/cmake-build-debug/SDL2_ttf")

# ===============================
# Tìm và liên kết SDL2
# ===============================
#find_package(SDL2 REQUIRED CONFIG PATHS ${SDL2_DIR})
#find_package(SDL2_image REQUIRED CONFIG PATHS ${SDL2_IMAGE_DIR})
#find_package(SDL2_mixer REQUIRED CONFIG PATHS ${SDL2_MIXER_DIR})
#find_package(SDL2_ttf REQUIRED CONFIG PATHS ${SDL2_TTF_DIR})

# ===============================
# Tự động lấy tất cả file .cpp trong src/
# ===============================
file(GLOB SRC_FILES ${CMAKE_SOURCE_DIR}/src/*.cpp)

# Tạo executable
add_executable(Vorax_Serpens ${SRC_FILES}
        src/main.cpp
        src/Game.cpp
        src/Game.hpp
        src/Snake.cpp
        src/Snake.hpp
        src/Food.cpp
        src/Food.hpp
        src/Renderer.cpp
        src/Renderer.hpp
        src/Config.cpp
        src/Config.hpp
)

# ===============================
# Thêm đường dẫn include SDL2
# ===============================
target_include_directories(Vorax_Serpens PRIVATE
        "D:/Clion/Vorax_Serpens/cmake-build-debug/SDL2/x86_64-w64-mingw32/include/SDL2"
        "D:/Clion/Vorax_Serpens/cmake-build-debug/SDL2_image/x86_64-w64-mingw32/include/SDL2"
        "D:/Clion/Vorax_Serpens/cmake-build-debug/SDL2_mixer/x86_64-w64-mingw32/include/SDL2"
        "D:/Clion/Vorax_Serpens/cmake-build-debug/SDL2_ttf/x86_64-w64-mingw32/include/SDL2"
)

# ===============================
# Liên kết thư viện SDL2
# ===============================
target_link_directories(Vorax_Serpens PRIVATE
        "D:/Clion/Vorax_Serpens/cmake-build-debug/SDL2/x86_64-w64-mingw32/lib"
        "D:/Clion/Vorax_Serpens/cmake-build-debug/SDL2_image/x86_64-w64-mingw32/lib"
        "D:/Clion/Vorax_Serpens/cmake-build-debug/SDL2_mixer/x86_64-w64-mingw32/lib"
        "D:/Clion/Vorax_Serpens/cmake-build-debug/SDL2_ttf/x86_64-w64-mingw32/lib"
)

target_link_libraries(Vorax_Serpens PRIVATE
        SDL2 SDL2_image SDL2_mixer SDL2_ttf
)
