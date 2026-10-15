#include "Game.h"
#include <algorithm>
#include <fstream>
#include <iostream>
#include <string>

namespace {
constexpr int kCell = 30;
constexpr int kBoardX = 30;
constexpr int kBoardY = 20;
constexpr int kWindowW = 520;
constexpr int kWindowH = 640;
constexpr const char* kHighScoreFile = "highscore.txt";

constexpr Color kPieceColors[kPieceCount] = {
    {0, 240, 240},  // I
    {240, 240, 0},  // O
    {170, 0, 240},  // T
    {0, 240, 0},    // S
    {240, 0, 0},    // Z
    {0, 80, 240},   // J
    {240, 160, 0},  // L
};
constexpr Color kBackground{18, 18, 28};
constexpr Color kWell{8, 8, 14};
constexpr Color kGrid{30, 30, 44};
constexpr Color kWhite{235, 235, 245};
constexpr Color kGray{140, 140, 160};

Color shade(Color c, float f) {
    auto s = [f](Uint8 v) { return static_cast<Uint8>(std::min(255.0f, v * f)); };
    return {s(c.r), s(c.g), s(c.b)};
}
}  // namespace

Game::Game() : renderer_("Tetris", kWindowW, kWindowH), rng_(std::random_device{}()) {
    loadHighScore();
    board_.reset();
}

int Game::run() {
    Uint32 last = SDL_GetTicks();
    while (running_) {
        SDL_Event e;
        while (SDL_PollEvent(&e)) handleEvent(e);

        const Uint32 now = SDL_GetTicks();
        update(now - last);
        last = now;

        render();
        SDL_Delay(1);
    }
    return 0;
}

void Game::handleEvent(const SDL_Event& e) {
    if (e.type == SDL_QUIT) { running_ = false; return; }
    if (e.type != SDL_KEYDOWN) return;

    const SDL_Keycode key = e.key.keysym.sym;
    const bool repeat = e.key.repeat != 0;

    if (key == SDLK_ESCAPE) { running_ = false; return; }

    switch (state_) {
        case GameState::Menu:
        case GameState::GameOver:
            if (key == SDLK_RETURN || key == SDLK_SPACE) startNewGame();
            break;
        case GameState::Paused:
            if (key == SDLK_p) state_ = GameState::Playing;
            break;
        case GameState::Playing:
            if (key == SDLK_p) { state_ = GameState::Paused; break; }
            if (key == SDLK_LEFT) tryMove(-1, 0);
            else if (key == SDLK_RIGHT) tryMove(1, 0);
            else if (key == SDLK_DOWN) {
                if (tryMove(0, 1)) { ++score_; fallTimer_ = 0; }
            } else if (!repeat) {
                if (key == SDLK_UP || key == SDLK_x) tryRotate(1);
                else if (key == SDLK_z) tryRotate(-1);
                else if (key == SDLK_SPACE) hardDrop();
            }
            break;
    }
}

void Game::update(Uint32 dtMs) {
    if (state_ != GameState::Playing) return;
    fallTimer_ += dtMs;
    const Uint32 interval = static_cast<Uint32>(dropIntervalMs());
    while (state_ == GameState::Playing && fallTimer_ >= interval) {
        fallTimer_ -= interval;
        if (!tryMove(0, 1)) {
            lockPiece();
            fallTimer_ = 0;
            break;
        }
    }
}

void Game::startNewGame() {
    board_.reset();
    bag_.clear();
    score_ = 0;
    lines_ = 0;
    level_ = 1;
    fallTimer_ = 0;
    next_ = nextFromBag();
    state_ = GameState::Playing;
    spawnPiece();
}

PieceType Game::nextFromBag() {
    if (bag_.empty()) {
        for (int i = 0; i < kPieceCount; ++i) bag_.push_back(static_cast<PieceType>(i));
        std::shuffle(bag_.begin(), bag_.end(), rng_);
    }
    const PieceType t = bag_.back();
    bag_.pop_back();
    return t;
}

void Game::spawnPiece() {
    current_ = next_;
    next_ = nextFromBag();
    rot_ = 0;
    x_ = 3;
    y_ = 0;
    if (board_.collides(current_, rot_, x_, y_)) {
        state_ = GameState::GameOver;
        if (score_ > highScore_) {
            highScore_ = score_;
            saveHighScore();
        }
    }
}

bool Game::tryMove(int dx, int dy) {
    if (board_.collides(current_, rot_, x_ + dx, y_ + dy)) return false;
    x_ += dx;
    y_ += dy;
    return true;
}

void Game::tryRotate(int dir) {
    if (current_ == PieceType::O) return;
    const int newRot = ((rot_ + dir) % 4 + 4) % 4;
    // Simplified wall kicks: try nudging sideways (and up) until it fits.
    static constexpr Point kicks[] = {{0, 0}, {-1, 0}, {1, 0}, {-2, 0}, {2, 0}, {0, -1}};
    for (const auto& k : kicks) {
        if (!board_.collides(current_, newRot, x_ + k.x, y_ + k.y)) {
            rot_ = newRot;
            x_ += k.x;
            y_ += k.y;
            return;
        }
    }
}

int Game::ghostY() const {
    int gy = y_;
    while (!board_.collides(current_, rot_, x_, gy + 1)) ++gy;
    return gy;
}

void Game::hardDrop() {
    const int gy = ghostY();
    score_ += 2 * (gy - y_);
    y_ = gy;
    lockPiece();
    fallTimer_ = 0;
}

void Game::lockPiece() {
    board_.lock(current_, rot_, x_, y_);
    const int cleared = board_.clearLines();
    if (cleared > 0) {
        static constexpr int kPoints[] = {0, 100, 300, 500, 800};
        score_ += kPoints[cleared] * level_;
        lines_ += cleared;
        level_ = lines_ / 10 + 1;
    }
    spawnPiece();
}

int Game::dropIntervalMs() const {
    return std::max(60, 800 - (level_ - 1) * 70);
}

void Game::loadHighScore() {
    std::ifstream in(kHighScoreFile);
    if (!(in >> highScore_)) highScore_ = 0;  // missing or corrupt file -> 0
}

void Game::saveHighScore() const {
    std::ofstream out(kHighScoreFile, std::ios::trunc);
    if (!out) {
        std::cerr << "Warning: could not write " << kHighScoreFile << "\n";
        return;
    }
    out << highScore_ << "\n";
}

void Game::drawCell(int px, int py, PieceType t, bool ghost) {
    const Color base = kPieceColors[static_cast<int>(t)];
    if (ghost) {
        renderer_.fillRect(px, py, kCell, kCell, shade(base, 0.25f));
        renderer_.fillRect(px + 3, py + 3, kCell - 6, kCell - 6, kWell);
        return;
    }
    renderer_.fillRect(px, py, kCell, kCell, shade(base, 0.55f));
    renderer_.fillRect(px + 2, py + 2, kCell - 4, kCell - 4, base);
}

void Game::drawPanel(const std::string& label, const std::string& value, int y) {
    renderer_.drawText(label, 360, y, 3, kGray);
    renderer_.drawText(value, 360, y + 24, 4, kWhite);
}

void Game::render() {
    renderer_.clear(kBackground);

    // Well and grid lines
    constexpr int boardW = Board::kWidth * kCell;
    constexpr int boardH = Board::kHeight * kCell;
    renderer_.fillRect(kBoardX, kBoardY, boardW, boardH, kWell);
    for (int x = 0; x <= Board::kWidth; ++x)
        renderer_.fillRect(kBoardX + x * kCell, kBoardY, 1, boardH, kGrid);
    for (int y = 0; y <= Board::kHeight; ++y)
        renderer_.fillRect(kBoardX, kBoardY + y * kCell, boardW, 1, kGrid);

    // Locked blocks
    for (int y = 0; y < Board::kHeight; ++y)
        for (int x = 0; x < Board::kWidth; ++x)
            if (const int v = board_.at(x, y))
                drawCell(kBoardX + x * kCell, kBoardY + y * kCell, static_cast<PieceType>(v - 1));

    // Ghost + active piece
    if (state_ == GameState::Playing || state_ == GameState::Paused) {
        const int gy = ghostY();
        for (const auto& c : cellsOf(current_, rot_)) {
            if (gy + c.y >= 0)
                drawCell(kBoardX + (x_ + c.x) * kCell, kBoardY + (gy + c.y) * kCell, current_, true);
        }
        for (const auto& c : cellsOf(current_, rot_)) {
            if (y_ + c.y >= 0)
                drawCell(kBoardX + (x_ + c.x) * kCell, kBoardY + (y_ + c.y) * kCell, current_);
        }
    }

    // Sidebar
    drawPanel("SCORE", std::to_string(score_), 30);
    drawPanel("LINES", std::to_string(lines_), 110);
    drawPanel("LEVEL", std::to_string(level_), 190);
    drawPanel("BEST", std::to_string(highScore_), 270);

    renderer_.drawText("NEXT", 360, 360, 3, kGray);
    renderer_.fillRect(360, 385, 120, 90, kWell);
    for (const auto& c : cellsOf(next_, 0)) {
        const int off = (next_ == PieceType::I) ? 0 : 15;
        drawCell(370 + off + c.x * 24 - (kCell - 24) / 2 + 0, 395 + c.y * 24, next_);
    }

    renderer_.drawText("ARROWS MOVE", 360, 500, 2, kGray);
    renderer_.drawText("UP X ROTATE", 360, 520, 2, kGray);
    renderer_.drawText("Z ROTATE BACK", 360, 540, 2, kGray);
    renderer_.drawText("SPACE DROP", 360, 560, 2, kGray);
    renderer_.drawText("P PAUSE", 360, 580, 2, kGray);
    renderer_.drawText("ESC QUIT", 360, 600, 2, kGray);

    // Overlays
    auto overlay = [&](const std::string& title, const std::string& sub) {
        renderer_.fillRect(kBoardX, 240, boardW, 120, {0, 0, 0, 255});
        const int tw = static_cast<int>(title.size()) * 4 * 5;
        renderer_.drawText(title, kBoardX + (boardW - tw) / 2, 262, 5, kWhite);
        const int sw = static_cast<int>(sub.size()) * 4 * 2;
        renderer_.drawText(sub, kBoardX + (boardW - sw) / 2, 320, 2, kGray);
    };
    if (state_ == GameState::Menu) overlay("TETRIS", "PRESS ENTER TO START");
    else if (state_ == GameState::Paused) overlay("PAUSED", "PRESS P TO RESUME");
    else if (state_ == GameState::GameOver) overlay("GAME OVER", "PRESS ENTER TO RESTART");

    renderer_.present();
}
