#include "Renderer.h"
#include <cctype>
#include <stdexcept>

namespace {
// 3x5 glyphs, row by row, '1' = filled. Index 0-9 = digits, 10-35 = A-Z.
constexpr const char* kGlyphs[36] = {
    "111101101101111", "010110010010111", "111001111100111", "111001111001111",
    "101101111001001", "111100111001111", "111100111101111", "111001001001001",
    "111101111101111", "111101111001111",
    "010101111101101", "110101110101110", "011100100100011", "110101101101110",
    "111100110100111", "111100110100100", "011100101101011", "101101111101101",
    "111010010010111", "001001001101010", "101101110101101", "100100100100111",
    "101111111101101", "110101101101101", "010101101101010", "110101110100100",
    "010101101111011", "110101110101101", "011100010001110", "111010010010010",
    "101101101101111", "101101101101010", "101101111111101", "101101010101101",
    "101101010010010", "111001010100111"};
}  // namespace

Renderer::Renderer(const std::string& title, int width, int height) {
    SDL_Window* w = SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_CENTERED,
                                     SDL_WINDOWPOS_CENTERED, width, height, 0);
    if (!w) throw std::runtime_error(std::string("SDL_CreateWindow: ") + SDL_GetError());
    window_.reset(w);

    SDL_Renderer* r = SDL_CreateRenderer(w, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!r) r = SDL_CreateRenderer(w, -1, SDL_RENDERER_SOFTWARE);
    if (!r) throw std::runtime_error(std::string("SDL_CreateRenderer: ") + SDL_GetError());
    renderer_.reset(r);
}

void Renderer::clear(Color c) {
    SDL_SetRenderDrawColor(renderer_.get(), c.r, c.g, c.b, c.a);
    SDL_RenderClear(renderer_.get());
}

void Renderer::fillRect(int x, int y, int w, int h, Color c) {
    SDL_SetRenderDrawColor(renderer_.get(), c.r, c.g, c.b, c.a);
    const SDL_Rect rect{x, y, w, h};
    SDL_RenderFillRect(renderer_.get(), &rect);
}

void Renderer::drawText(const std::string& text, int x, int y, int scale, Color c) {
    int cursor = x;
    for (char ch : text) {
        const unsigned char u = static_cast<unsigned char>(ch);
        const char* glyph = nullptr;
        if (std::isdigit(u)) glyph = kGlyphs[u - '0'];
        else if (std::isalpha(u)) glyph = kGlyphs[10 + (std::toupper(u) - 'A')];
        if (glyph) {
            for (int i = 0; i < 15; ++i) {
                if (glyph[i] == '1') {
                    fillRect(cursor + (i % 3) * scale, y + (i / 3) * scale, scale, scale, c);
                }
            }
        }
        cursor += 4 * scale;
    }
}

void Renderer::present() { SDL_RenderPresent(renderer_.get()); }
