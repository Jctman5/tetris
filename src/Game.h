#pragma once
#include <SDL.h>
#include <random>
#include <vector>
#include "Board.h"
#include "Renderer.h"
#include "Tetromino.h"

enum class GameState { Menu, Playing, Paused, GameOver };

class Game {
public:
    Game();
    int run();

private:
    // loop pieces
    void handleEvent(const SDL_Event& e);
    void update(Uint32 dtMs);
    void render();

    // game logic
    void startNewGame();
    void spawnPiece();
    bool tryMove(int dx, int dy);
    void tryRotate(int dir);
    void hardDrop();
    void lockPiece();
    int ghostY() const;
    int dropIntervalMs() const;
    PieceType nextFromBag();

    // persistence
    void loadHighScore();
    void saveHighScore() const;

    // drawing helpers
    void drawCell(int px, int py, PieceType t, bool ghost = false);
    void drawPanel(const std::string& label, const std::string& value, int y);

    Renderer renderer_;
    Board board_;
    GameState state_ = GameState::Menu;
    bool running_ = true;

    PieceType current_ = PieceType::T;
    PieceType next_ = PieceType::T;
    int rot_ = 0, x_ = 3, y_ = 0;

    std::vector<PieceType> bag_;
    std::mt19937 rng_;

    int score_ = 0, lines_ = 0, level_ = 1, highScore_ = 0;
    Uint32 fallTimer_ = 0;
};
