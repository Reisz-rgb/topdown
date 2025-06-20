#ifndef OPTIONS_MENU_H
#define OPTIONS_MENU_H

#include "raylib.h"

typedef enum {
    WINDOW_ORIGINAL,
    WINDOW_MAXIMIZED,
    WINDOW_FULLSCREEN
} WindowMode;

typedef struct {
    float musicVolume;      // 0.0f - 1.0f
    float sfxVolume;        // 0.0f - 1.0f
    WindowMode windowMode;  // Track current window mode
    int selectedOption;     // 0 = music, 1 = sfx, 2 = window size
} OptionsMenuState;

void InitOptionsMenu(OptionsMenuState* state);
bool UpdateOptionsMenu(OptionsMenuState* state); // Returns true when should go back
void DrawOptionsMenu(OptionsMenuState* state);

#endif
