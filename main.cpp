#include <GL/gl.h>
#include <GL/glut.h>
#include <windows.h>
#include <mmsystem.h>
#include <stdlib.h>
#include <cstdio>
#include <ctime>
#include "grid.h"

#define COLUMNS 40
#define ROWS 40

bool difficultySelected = false;
bool showSelectWarning = false;

// State tracking variables
int FPS = 10;
int gameState = STATE_MENU;
int score = 0;
int highScore = 0;
extern short sDirection;

void timer(int);
void display();
void reshape(int, int);
void keyboardSpecial(int, int, int);
void keyboardNormal(unsigned char, int, int);

void init() {
    glClearColor(0.02f, 0.02f, 0.05f, 1.0f); // Clean Cyberpunk Midnight Base
    initGrid(COLUMNS, ROWS);
}

int main(int argc, char **argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
    glutInitWindowSize(600, 600);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("SNAKE ARENA: EXTENDED EDITION");

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutTimerFunc(0, timer, 0);

    glutSpecialFunc(keyboardSpecial);   // Handle arrow keys
    glutKeyboardFunc(keyboardNormal);

    srand(time(0));

    init();
    glutMainLoop();
    return 0;
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT);

    switch(gameState) {
        case STATE_MENU:
            drawMenuScreen();
            break;
        case STATE_PLAYING:
            drawGrid();
            drawSnake();
            drawFood();

            // On-screen Real-time Dashboard HUD
            char hudText[50];
            sprintf(hudText, "Score:%d | High:%d | Speed:%d", score, highScore, FPS);
            drawText(hudText, 1.0f, ROWS - 1.5f, GLUT_BITMAP_HELVETICA_12, 0.0f, 1.0f, 1.0f);
            break;
        case STATE_PAUSED:
            drawGrid();
            drawPauseScreen();
            break;
        case STATE_GAMEOVER:
            drawGameOverScreen();
            break;
    }

    glutSwapBuffers();
}

void reshape(int w, int h) {
    glViewport(0, 0, (GLsizei)w, (GLsizei)h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0.0, COLUMNS, 0.0, ROWS, -1.0, 1.0);
    glMatrixMode(GL_MODELVIEW);
}

void timer(int v) {
    if(gameState == STATE_PLAYING) {
        updateSnake();
    }

    glutPostRedisplay();
    glutTimerFunc(1000 / FPS, timer, 0);
}

void keyboardNormal(unsigned char key, int x, int y) {
    switch(key) {
        case ' ':
            if (gameState == STATE_MENU) {
                if (difficultySelected) {
                    resetGame(); // Transitions to STATE_PLAYING inside reset
                } else {
                    showSelectWarning = true;
                }
            }
            break;
        case '1':
            if (gameState == STATE_MENU) {
                FPS = 8;
                difficultySelected = true;
                showSelectWarning = false;
            }
            break;
        case '2':
            if (gameState == STATE_MENU) {
                FPS = 12;
                difficultySelected = true;
                showSelectWarning = false;
            }
            break;
        case '3':
            if (gameState == STATE_MENU) {
                FPS = 18;
                difficultySelected = true;
                showSelectWarning = false;
            }
            break;
        case 'p':
        case 'P': // Pause Toggle
            if (gameState == STATE_PLAYING) {
                gameState = STATE_PAUSED;
            } else if (gameState == STATE_PAUSED) {
                gameState = STATE_PLAYING;
            }
            break;
        case 'r':
        case 'R': // Reset System and send to Menu
            if (gameState == STATE_GAMEOVER) {
                gameState = STATE_MENU;
                difficultySelected = false; // Prompt select speed again
            }
            break;
        case 27: // Escape key
            exit(0);
            break;
    }
}

void keyboardSpecial(int key, int x, int y) {
    if (gameState != STATE_PLAYING) return;

    switch(key) {
        case GLUT_KEY_UP:
            if(sDirection != DOWN) sDirection = UP;
            break;
        case GLUT_KEY_DOWN:
            if(sDirection != UP) sDirection = DOWN;
            break;
        case GLUT_KEY_RIGHT:
            if(sDirection != LEFT) sDirection = RIGHT;
            break;
        case GLUT_KEY_LEFT:
            if(sDirection != RIGHT) sDirection = LEFT;
            break;
    }
}
