#ifndef MAIN_MENU_H
#define MAIN_MENU_H

#include "raylib.h"

typedef enum {
    MENU_RESULT_NONE,
    MENU_RESULT_START_GAME,
    MENU_RESULT_OPTIONS,
    MENU_RESULT_EXIT
} MenuResult;

typedef struct {
    int selectedItem;
    float blinkTimer;
} MainMenuState;

void InitMainMenu(MainMenuState* state);
MenuResult UpdateMainMenu(MainMenuState* state);
void DrawMainMenu(MainMenuState* state);

#endif