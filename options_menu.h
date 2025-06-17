#ifndef OPTIONS_MENU_H
#define OPTIONS_MENU_H

#include "raylib.h"
#include "resources.h"

typedef struct {
    float musicVolume;
    float sfxVolume;
    int windowMode; // 0 = original, 1 = maximized, 2 = fullscreen
    int selectedOption;
    bool settingsChanged;
} OptionsMenuState;

void InitOptionsMenu(OptionsMenuState* state);
bool UpdateOptionsMenu(OptionsMenuState* state); // Returns true when should go back
void DrawOptionsMenu(OptionsMenuState* state);

#endif