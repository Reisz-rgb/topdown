#ifndef TITLE_SCREEN_H
#define TITLE_SCREEN_H

#include "raylib.h"

typedef struct {
    float blinkTimer;
    float alpha;
    bool fadeOut;
} TitleScreenState;

void InitTitleScreen(TitleScreenState* state);
void UpdateTitleScreen(TitleScreenState* state, bool* shouldStartGame);
void DrawTitleScreen(TitleScreenState* state);

#endif