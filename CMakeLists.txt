cmake_minimum_required(VERSION 3.30)
project(Vorax_Serpens LANGUAGES CXX RC)

set(CMAKE_CXX_STANDARD 20)

add_definitions(-DSDL_MAIN_HANDLED)

set(SDL2_BASE_DIR "${CMAKE_SOURCE_DIR}/libs")

set(SDL2_DIR "${SDL2_BASE_DIR}/SDL2")
set(SDL2_IMAGE_DIR "${SDL2_BASE_DIR}/SDL2_image")
set(SDL2_MIXER_DIR "${SDL2_BASE_DIR}/SDL2_mixer")
set(SDL2_TTF_DIR "${SDL2_BASE_DIR}/SDL2_ttf")

set(SRC_FILES
        src/main.cpp
        src/Game.cpp
        src/Snake.cpp
        src/Food.cpp
        src/Renderer.cpp
        src/Config.cpp
)

if(WIN32)
    set(WINDOWS_RESOURCE_FILE ${CMAKE_SOURCE_DIR}/vorax_serpens_resources.rc)
    if(EXISTS ${WINDOWS_RESOURCE_FILE})
        list(APPEND SRC_FILES ${WINDOWS_RESOURCE_FILE})
        message(STATUS "Windows resource file for icon found: ${WINDOWS_RESOURCE_FILE}")
    else()
        message(WARNING "Windows resource file for icon NOT found: ${WINDOWS_RESOURCE_FILE}. No custom icon will be embedded.")
    endif()
endif()

add_executable(Vorax_Serpens WIN32 ${SRC_FILES})

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

target_link_libraries(Vorax_Serpens PRIVATE
        mingw32
        SDL2main
        SDL2 SDL2_image SDL2_mixer SDL2_ttf
)

set(ASSET_SOURCE_DIR ${CMAKE_SOURCE_DIR}/assets)
set(ASSET_DESTINATION_DIR ${CMAKE_BINARY_DIR}/assets)

add_custom_command(TARGET Vorax_Serpens POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${ASSET_SOURCE_DIR}"
        "${ASSET_DESTINATION_DIR}"
)
