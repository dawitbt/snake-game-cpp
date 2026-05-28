#include <GL/gl.h>
#include <GL/glut.h>
#include <ctime>
#include <cstdio>     // For file tracking (high score)
#include <cstring>
#include <windows.h>  // Required for PlaySound
#include <mmsystem.h>
#include "grid.h"


extern int score;
extern int highScore;
extern int gameState;
extern int FPS;

int gridX, gridY;
int s_length = 5;
bool food = true;
int foodX, foodY;
short sDirection = RIGHT;

// Snake Position Arrays
int posX[60] = {20, 20, 20, 20, 20};
int posY[60] = {20, 19, 18, 17, 16};

// Initialization
void initGrid(int x, int y) {
    gridX = x;
    gridY = y;
    loadHighScore();
}

// Bitmap Text Engine
void drawText(const char* text, float x, float y, void* font, float r, float g, float b) {
    glColor3f(r, g, b);
    glRasterPos2f(x, y);
    for (int i = 0; text[i] != '\0'; i++) {
        glutBitmapCharacter(font, text[i]);
    }
}

void unit(int, int);

// Upgraded Retro-Futuristic Grid Background
void drawGrid() {
    for (int x = 0; x < gridX; x++) {
        for (int y = 0; y < gridY; y++) {
            unit(x, y);
        }
    }
}

void unit(int x, int y) {
    if (x == 0 || y == 0 || x == gridX - 1 || y == gridY - 1) {
        glLineWidth(4.0);
        glColor3f(0.8f, 0.1f, 0.1f); // Sleek deep red boundary
    } else {
        glLineWidth(1.0);
        glColor3f(0.05f, 0.15f, 0.05f); // Subtle, dark-green matrix grid lines
    }

    glBegin(GL_LINE_LOOP);
        glVertex2f(x, y);
        glVertex2f(x + 1, y);
        glVertex2f(x + 1, y + 1);
        glVertex2f(x, y + 1);
    glEnd();
}

// Stylized Spherical/Glowing Food
void drawFood() {
    if (food) {
        random(foodX, foodY);
        food = false;
    }

    // Draw a detailed glowing target food item using overlapping primitives
    glColor3f(1.0f, 0.2f, 0.2f); // Outer shell
    glRectf(foodX + 0.1f, foodY + 0.1f, foodX + 0.9f, foodY + 0.9f);
    glColor3f(1.0f, 0.8f, 0.0f); // Bright core
    glRectf(foodX + 0.3f, foodY + 0.3f, foodX + 0.7f, foodY + 0.7f);
}

// Upgraded Snake Drawing with Gradient Mechanics
void drawSnake() {
    for (int i = s_length - 1; i > 0; i--) {
        posX[i] = posX[i - 1];
        posY[i] = posY[i - 1];
    }

    if (sDirection == UP)        posY[0]++;
    else if (sDirection == DOWN)  posY[0]--;
    else if (sDirection == RIGHT) posX[0]++;
    else if (sDirection == LEFT)  posX[0]--;

    // Collision Detection: Walls
    if (posX[0] <= 0 || posX[0] >= gridX - 1 || posY[0] <= 0 || posY[0] >= gridY - 1) {
        PlaySound(TEXT("hit.wav"), NULL, SND_ASYNC | SND_FILENAME);
        gameState = STATE_GAMEOVER;
        saveHighScore();
        return;
    }

    // Collision Detection: Self-cannibalism
    for(int i = 1; i < s_length; i++) {
        if(posX[0] == posX[i] && posY[0] == posY[i]) {
            PlaySound(TEXT("hit.wav"), NULL, SND_ASYNC | SND_FILENAME);
            gameState = STATE_GAMEOVER;
            saveHighScore();
            return;
        }
    }

    // Draw Snake with a dynamic color gradient
    for (int i = 0; i < s_length; i++) {
        if (i == 0) {
            glColor3f(0.0f, 1.0f, 0.5f); // Neon green head
        } else {
            // Smoothly transition body segments into cyan blue tones
            float t = (float)i / s_length;
            glColor3f(0.0f, 1.0f - (t * 0.5f), 0.5f + (t * 0.5f));
        }
        // Rounded inset illusion
        glRectf(posX[i] + 0.05f, posY[i] + 0.05f, posX[i] + 0.95f, posY[i] + 0.95f);
    }

    // Food consumption tracking
    if (posX[0] == foodX && posY[0] == foodY) {
        PlaySound(TEXT("hiss.wav"), NULL, SND_ASYNC | SND_FILENAME);
        score++;
        s_length++;
        if (s_length > MAX) s_length = MAX;
        food = true;

        // Level Up & Speed Increments
        if (score % 3 == 0 && FPS < 25) {
            FPS += 2; // Increase speed every 3 points eaten
        }
    }
}

void random(int &x, int &y) {
    int maxX = gridX - 2;
    int maxY = gridY - 2;
    int minV = 1;
    srand(time(NULL));

    bool onSnake;
    do {
        onSnake = false;
        x = minV + rand() % (maxX - minV + 1);
        y = minV + rand() % (maxY - minV + 1);

        // Ensure food does not spawn directly inside the snake's body
        for(int i = 0; i < s_length; i++) {
            if(posX[i] == x && posY[i] == y) {
                onSnake = true;
                break;
            }
        }
    } while(onSnake);
}

// --- Menu & Screen State UI Displays ---

void drawMenuScreen() {
    drawText("RETRO SNAKE GAME", 8.0f, 25.0f, GLUT_BITMAP_TIMES_ROMAN_24, 0.0f, 1.0f, 0.6f);
    drawText("Developed by: Your Name", 11.0f, 20.0f, GLUT_BITMAP_HELVETICA_12, 0.7f, 0.7f, 0.7f);
    drawText("Press [SPACE] to Start Game", 9.5f, 14.0f, GLUT_BITMAP_HELVETICA_18, 1.0f, 1.0f, 1.0f);
    drawText("Controls: Arrow Keys to Move, [P] to Pause", 6.0f, 8.0f, GLUT_BITMAP_HELVETICA_12, 0.5f, 0.6f, 0.8f);
}

void drawPauseScreen() {
    drawText("GAME PAUSED", 13.5f, 22.0f, GLUT_BITMAP_TIMES_ROMAN_24, 1.0f, 0.8f, 0.0f);
    drawText("Press [P] to Resume Gameplay", 10.5f, 17.0f, GLUT_BITMAP_HELVETICA_12, 1.0f, 1.0f, 1.0f);
}

void drawGameOverScreen() {
    drawText("GAME OVER", 14.0f, 26.0f, GLUT_BITMAP_TIMES_ROMAN_24, 1.0f, 0.2f, 0.2f);

    char scoreStr[30], highStr[30];
    sprintf(scoreStr, "Final Score: %d", score);
    sprintf(highStr, "Personal High Score: %d", highScore);

    drawText(scoreStr, 16.0f, 20.0f, GLUT_BITMAP_HELVETICA_18, 1.0f, 1.0f, 1.0f);
    drawText(highStr, 13.0f, 16.0f, GLUT_BITMAP_HELVETICA_12, 1.0f, 0.84f, 0.0f);
    drawText("Press [R] to Restart / Reset Arena", 10.0f, 10.0f, GLUT_BITMAP_HELVETICA_12, 0.4f, 0.8f, 1.0f);
}

void resetGame() {
    score = 0;
    s_length = 5;
    food = true;
    sDirection = RIGHT;
    FPS = 10; // Reset speed index

    // Reset positioning
    int startX = 20, startY = 20;
    for (int i = 0; i < 60; i++) {
        if(i < s_length) {
            posX[i] = startX;
            posY[i] = startY - i;
        } else {
            posX[i] = 0;
            posY[i] = 0;
        }
    }
    gameState = STATE_PLAYING;
}

// --- Persistence Record Engine ---

void loadHighScore() {
    FILE* file = fopen("highscore.txt", "r");
    if (file) {
        fscanf(file, "%d", &highScore);
        fclose(file);
    } else {
        highScore = 0;
    }
}

void saveHighScore() {
    if (score > highScore) {
        highScore = score;
        FILE* file = fopen("highscore.txt", "w");
        if (file) {
            fprintf(file, "%d", highScore);
            fclose(file);
        }
    }
}

