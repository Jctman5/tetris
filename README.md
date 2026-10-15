# Overview

As a software engineer, I wanted to get comfortable with C++ by building something more demanding than small exercises: a complete, playable program where memory, structure, and performance all matter. I chose a Tetris clone because it forces you to handle real-time input, a game loop, collision detection, state management, and file storage, all in one project.

This is a desktop Tetris clone written in C++17 with SDL2 for the window, rendering, and keyboard input. It includes:

* The standard 10x20 board with all seven tetrominoes (I, O, T, S, Z, J, L)
* A 7-bag randomizer, so every piece appears once per seven drops
* Rotation in both directions with simple wall kicks
* Soft drop, hard drop, and a ghost piece showing where the piece will land
* Line clearing, scoring, and levels that speed up the game
* A next-piece preview
* Menu, pause, and game over screens
* A high score saved to a file between sessions

My purpose for building this was to practice core C++ concepts in a real project: classes and encapsulation, RAII and smart pointers for resource management, STL containers and algorithms, enums, lambdas, file I/O, header/source separation, and building with CMake.

[Software Demo Video](https://www.youtube.com/watch?v=4e7rbB10pZg)

# Development Environment

* **Editor:** Visual Studio Code with the C/C++ extension
* **Compiler and shell:** MSYS2 (UCRT64) with GCC
* **Build system:** CMake with Ninja
* **Graphics and input library:** SDL2
* **Debugging:** compiler warnings (`-Wall -Wextra -Wpedantic`) and AddressSanitizer for catching memory errors
* **Version control:** Git

The program is written in **C++17**. It uses modern features such as `enum class`, `std::unique_ptr` with custom deleters, `std::array`, `std::vector`, `std::shuffle` with `<random>`, lambdas, `constexpr`, and `<fstream>` for file handling.

# Useful Websites

* [SDL2 Wiki](https://wiki.libsdl.org/SDL2/FrontPage)
* [Lazy Foo' SDL Tutorials](https://lazyfoo.net/tutorials/SDL/)
* [cppreference.com](https://en.cppreference.com/)
* [LearnCpp.com](https://www.learncpp.com/)
* [CMake Documentation](https://cmake.org/cmake/help/latest/)
* [MSYS2](https://www.msys2.org/)
* [Tetris Wiki: Super Rotation System](https://tetris.wiki/Super_Rotation_System)