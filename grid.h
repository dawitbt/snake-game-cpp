#ifndef GRID_H_INCLUDED
#define GRID_H_INCLUDED

// Direction Macros
#define UP 1
#define DOWN -1
#define RIGHT 2
#define LEFT -2
#define MAX 60

// Game States
#define STATE_MENU 0
#define STATE_PLAYING 1
#define STATE_PAUSED 2
#define STATE_GAMEOVER 3

// Core Globals referenced across files
extern bool showSelectWarning;
extern bool difficultySelected;

// Core Functions
void initGrid(int, int);
void drawGrid();
void drawSnake();
void updateSnake();
void drawFood();
void random(int&, int&);

// UI and Text Rendering Functions
void drawText(const char* text, float x, float y, void* font, float r, float g, float b);
void drawMenuScreen();
void drawPauseScreen();
void drawGameOverScreen();
void resetGame();

// File I/O for High Scores
void loadHighScore();
void saveHighScore();

#endif // GRID_H_INCLUDED
