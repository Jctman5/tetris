#pragma once
#include <array>
#include "Tetromino.h"

class Board {
public:
    static constexpr int kWidth = 10;
    static constexpr int kHeight = 20;

    void reset();

    // True if the piece would overlap a wall, the floor, or a locked block.
    bool collides(PieceType type, int rot, int px, int py) const;

    // Writes the piece into the grid.
    void lock(PieceType type, int rot, int px, int py);

    // Removes full rows, returns how many were cleared.
    int clearLines();

    // 0 = empty, otherwise (PieceType + 1).
    int at(int x, int y) const { return grid_[y][x]; }

private:
    std::array<std::array<int, kWidth>, kHeight> grid_{};
};
