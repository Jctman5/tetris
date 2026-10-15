#pragma once
#include <SDL.h>
#include <memory>
#include <string>

struct Color {
    Uint8 r, g, b, a = 255;
};

// Owns the SDL window and renderer (RAII via unique_ptr custom deleters).
class Renderer {
public:
    Renderer(const std::string& title, int width, int height);

    void clear(Color c);
    void fillRect(int x, int y, int w, int h, Color c);
    // Draws text with a built-in 3x5 pixel font (A-Z, 0-9, space). Each font
    // pixel becomes a `scale` x `scale` square.
    void drawText(const std::string& text, int x, int y, int scale, Color c);
    void present();

private:
    struct WindowDeleter {
        void operator()(SDL_Window* w) const { SDL_DestroyWindow(w); }
    };
    struct RendererDeleter {
        void operator()(SDL_Renderer* r) const { SDL_DestroyRenderer(r); }
    };
    std::unique_ptr<SDL_Window, WindowDeleter> window_;
    std::unique_ptr<SDL_Renderer, RendererDeleter> renderer_;
};
