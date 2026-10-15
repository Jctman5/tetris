#include <SDL.h>
#include <exception>
#include <iostream>
#include "Game.h"

int main(int /*argc*/, char* /*argv*/[]) {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        std::cerr << "SDL_Init failed: " << SDL_GetError() << "\n";
        return 1;
    }
    int result = 0;
    try {
        Game game;  // destroyed (window/renderer freed) before SDL_Quit
        result = game.run();
    } catch (const std::exception& ex) {
        std::cerr << "Fatal error: " << ex.what() << "\n";
        result = 1;
    }
    SDL_Quit();
    return result;
}
