cmake_minimum_required(VERSION 3.16)
project(Tetris LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  set(CMAKE_BUILD_TYPE Release)
endif()

find_package(SDL2 REQUIRED CONFIG)

add_executable(tetris
  src/main.cpp
  src/Board.cpp
  src/Game.cpp
  src/Renderer.cpp
)

if(MSVC)
  target_compile_options(tetris PRIVATE /W4)
else()
  target_compile_options(tetris PRIVATE -Wall -Wextra -Wpedantic)
endif()

# SDL2main is only provided on some platforms (e.g. Windows)
target_link_libraries(tetris PRIVATE
  $<TARGET_NAME_IF_EXISTS:SDL2::SDL2main>
  SDL2::SDL2
)
