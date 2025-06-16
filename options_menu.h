#ifndef OPTIONS_MENU_H
#define OPTIONS_MENU_H

#include "raylib.h"

typedef struct {
    // Add any options-specific state here
    // For example: volume levels, control settings, etc.
} OptionsMenuState;

void InitOptionsMenu(OptionsMenuState* state);
bool UpdateOptionsMenu(OptionsMenuState* state); // Returns true when should go back
void DrawOptionsMenu(OptionsMenuState* state);

#endif