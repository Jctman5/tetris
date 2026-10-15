#pragma once
#include <array>

enum class PieceType { I = 0, O, T, S, Z, J, L };
constexpr int kPieceCount = 7;

struct Point {
    int x;
    int y;
};

using Cells = std::array<Point, 4>;

// Returns the four cells (relative to the piece's top-left corner) of a piece
// in rotation state `rot` (0-3, clockwise). Rotations are computed from the
// spawn shape by rotating inside the piece's bounding box.
inline Cells cellsOf(PieceType type, int rot) {
    Cells base{};
    int size = 3;
    switch (type) {
        case PieceType::I: base = {{{0, 1}, {1, 1}, {2, 1}, {3, 1}}}; size = 4; break;
        case PieceType::O: return {{{1, 0}, {2, 0}, {1, 1}, {2, 1}}};  // never rotates
        case PieceType::T: base = {{{1, 0}, {0, 1}, {1, 1}, {2, 1}}}; break;
        case PieceType::S: base = {{{1, 0}, {2, 0}, {0, 1}, {1, 1}}}; break;
        case PieceType::Z: base = {{{0, 0}, {1, 0}, {1, 1}, {2, 1}}}; break;
        case PieceType::J: base = {{{0, 0}, {0, 1}, {1, 1}, {2, 1}}}; break;
        case PieceType::L: base = {{{2, 0}, {0, 1}, {1, 1}, {2, 1}}}; break;
    }
    rot = ((rot % 4) + 4) % 4;
    for (int r = 0; r < rot; ++r) {
        for (auto& c : base) {
            const int nx = size - 1 - c.y;
            const int ny = c.x;
            c = {nx, ny};
        }
    }
    return base;
}
