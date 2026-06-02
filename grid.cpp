#include <GL/gl.h>
#include <GL/glut.h>
#include <ctime>
#include <cstdio>
#include <cstring>
#include <windows.h>
#include <mmsystem.h>
#include "grid.h"

extern int score;
extern int highScore;
extern int gameState;
extern int FPS;
extern bool difficultySelected;

int gridX, gridY;
int s_length = 5;
bool food = true;
int foodX, foodY;
short sDirection = RIGHT;

// Snake Position Arrays
int posX[MAX] = {20, 20, 20, 20, 20};
int posY[MAX] = {20, 19, 18, 17, 16};

void initGrid(int x, int y) {
    gridX = x;
    gridY = y;
    loadHighScore();
}

void drawText(const char* text, float x, float y, void* font, float r, float g, float b) {
    glColor3f(r, g, b);
    glRasterPos2f(x, y);
    for (int i = 0; text[i] != '\0'; i++) {
        glutBitmapCharacter(font, text[i]);
    }
}

void unit(int x, int y) {
    if (x == 0 || y == 0 || x == gridX - 1 || y == gridY - 1) {
        glLineWidth(4.0);
        glColor3f(0.8f, 0.1f, 0.1f); // Sleek deep red boundary
    } else {
        glLineWidth(1.0);
        glColor3f(0.05f, 0.15f, 0.05f); // Dark-green matrix grid lines
    }

    glBegin(GL_LINE_LOOP);
        glVertex2f(x, y);
        glVertex2f(x + 1, y);
        glVertex2f(x + 1, y + 1);
        glVertex2f(x, y + 1);
    glEnd();
}

void drawGrid() {
    for (int x = 0; x < gridX; x++) {
        for (int y = 0; y < gridY; y++) {
            unit(x, y);
        }
    }
}

void drawFood() {
    if (food) {
        random(foodX, foodY);
        food = false;
    }
    glColor3f(1.0f, 0.2f, 0.2f); // Outer red shell
    glRectf(foodX + 0.1f, foodY + 0.1f, foodX + 0.9f, foodY + 0.9f);
    glColor3f(1.0f, 0.8f, 0.0f); // Bright yellow core
    glRectf(foodX + 0.3f, foodY + 0.3f, foodX + 0.7f, foodY + 0.7f);
}

void updateSnake() {
    // Move body
    for (int i = s_length - 1; i > 0; i--) {
        posX[i] = posX[i - 1];
        posY[i] = posY[i - 1];
    }

    // Move head
    if (sDirection == UP)        posY[0]++;
    else if (sDirection == DOWN)  posY[0]--;
    else if (sDirection == RIGHT) posX[0]++;
    else if (sDirection == LEFT)  posX[0]--;
}

void drawSnake() {
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
            float t = (float)i / s_length;
            glColor3f(0.0f, 1.0f - (t * 0.5f), 0.5f + (t * 0.5f));
        }
        glRectf(posX[i] + 0.05f, posY[i] + 0.05f, posX[i] + 0.95f, posY[i] + 0.95f);
    }

    // Food consumption tracking
    if (posX[0] == foodX && posY[0] == foodY) {
        PlaySound(TEXT("hiss.wav"), NULL, SND_ASYNC | SND_FILENAME);
        score++;
        s_length++;
        if (s_length > MAX) s_length = MAX;
        food = true;
    }
}

void random(int &x, int &y) {
    int maxX = gridX - 2;
    int maxY = gridY - 2;
    int minV = 1;
    bool onSnake;

    do {
        onSnake = false;
        x = minV + rand() % (maxX - minV + 1);
        y = minV + rand() % (maxY - minV + 1);

        for(int i = 0; i < s_length; i++) {
            if(posX[i] == x && posY[i] == y) {
                onSnake = true;
                break;
            }
        }
    } while(onSnake);
}

void drawMenuScreen() {
    drawText("SNAKE ARENA", 14.0f, 34.0f, GLUT_BITMAP_TIMES_ROMAN_24, 0.0f, 1.0f, 0.7f);
    drawText("Retro Snake Game", 14.0f, 31.0f, GLUT_BITMAP_HELVETICA_18, 0.8f, 0.8f, 0.8f);
    drawText("SELECT DIFFICULTY", 13.0f, 27.0f, GLUT_BITMAP_HELVETICA_18, 1.0f, 0.6f, 0.0f);

    drawText("1 - EASY", 15.0f, 24.0f, GLUT_BITMAP_HELVETICA_18, 0.0f, 1.0f, 0.0f);
    drawText("2 - MEDIUM", 15.0f, 22.0f, GLUT_BITMAP_HELVETICA_18, 1.0f, 1.0f, 0.0f);
    drawText("3 - HARD", 15.0f, 20.0f, GLUT_BITMAP_HELVETICA_18, 1.0f, 0.3f, 0.3f);

    drawText("Press SPACE to Start", 12.0f, 15.0f, GLUT_BITMAP_HELVETICA_18, 1.0f, 1.0f, 1.0f);
    drawText("Controls: Arrow Keys | P = Pause | R = Restart", 5.0f, 8.0f, GLUT_BITMAP_HELVETICA_12, 0.6f, 0.7f, 1.0f);
    drawText("Developed by: DAWIT", 14.0f, 5.0f, GLUT_BITMAP_HELVETICA_12, 0.5f, 0.5f, 0.5f);

    if (showSelectWarning) {
        drawText("PLEASE SELECT DIFFICULTY (1 / 2 / 3)", 8.0f, 12.0f, GLUT_BITMAP_HELVETICA_18, 1.0f, 0.2f, 0.2f);
    }
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
    drawText("Press [R] to Return to Menu", 11.5f, 10.0f, GLUT_BITMAP_HELVETICA_12, 0.4f, 0.8f, 1.0f);
}

void resetGame() {
    score = 0;
    s_length = 5;
    food = true;
    sDirection = RIGHT;

    int startX = 20, startY = 20;
    for (int i = 0; i < MAX; i++) {
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

void loadHighScore() {
    FILE* file = fopen("highscore.txt", "r");
    if (file) {
        if (fscanf(file, "%d", &highScore) != 1) {
            highScore = 0;
        }
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
