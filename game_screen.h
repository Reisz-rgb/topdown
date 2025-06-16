#ifndef GAME_SCREEN_H
#define GAME_SCREEN_H

#include "raylib.h"

typedef struct {
    // Add any game-specific state here
    // For example: player position, game objects, etc.
} GameScreenState;

void InitGameScreen(GameScreenState* state);
bool UpdateGameScreen(GameScreenState* state); // Returns true when should go back
void DrawGameScreen(GameScreenState* state);

#endif