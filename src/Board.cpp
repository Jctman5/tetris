#include "Board.h"
#include <algorithm>

void Board::reset() {
    for (auto& row : grid_) row.fill(0);
}

bool Board::collides(PieceType type, int rot, int px, int py) const {
    for (const auto& c : cellsOf(type, rot)) {
        const int x = px + c.x;
        const int y = py + c.y;
        if (x < 0 || x >= kWidth || y >= kHeight) return true;
        if (y >= 0 && grid_[y][x] != 0) return true;
    }
    return false;
}

void Board::lock(PieceType type, int rot, int px, int py) {
    for (const auto& c : cellsOf(type, rot)) {
        const int x = px + c.x;
        const int y = py + c.y;
        if (y >= 0 && y < kHeight && x >= 0 && x < kWidth) {
            grid_[y][x] = static_cast<int>(type) + 1;
        }
    }
}

int Board::clearLines() {
    int cleared = 0;
    int writeRow = kHeight - 1;
    for (int readRow = kHeight - 1; readRow >= 0; --readRow) {
        const bool full = std::all_of(grid_[readRow].begin(), grid_[readRow].end(),
                                      [](int v) { return v != 0; });
        if (full) {
            ++cleared;
        } else {
            if (writeRow != readRow) grid_[writeRow] = grid_[readRow];
            --writeRow;
        }
    }
    for (int y = writeRow; y >= 0; --y) grid_[y].fill(0);
    return cleared;
}
