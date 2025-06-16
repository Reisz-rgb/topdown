// options_menu.c
#include "options_menu.h"
#include "resources.h"

void InitOptionsMenu(OptionsMenuState* state) {
    // Initialize any options state here
}

bool UpdateOptionsMenu(OptionsMenuState* state) {
    if (IsKeyPressed(KEY_Z)) {
        if (selectSfx.frameCount > 0) PlaySound(selectSfx);
        return true;
    }
    return false;
}

void DrawOptionsMenu(OptionsMenuState* state) {
    // Draw title
    const char* title = "OPTIONS";
    Vector2 titlePos = {
        SCREEN_WIDTH/2 - MeasureTextEx(gameFont, title, 30, 2).x/2,
        40
    };
    DrawTextEx(gameFont, title, (Vector2){titlePos.x + 2, titlePos.y + 2}, 30, 2, BLACK);
    DrawTextEx(gameFont, title, titlePos, 30, 2, WHITE);

    // Draw back instruction
    const char* backText = "Press Z to return";
    Vector2 backPos = {
        SCREEN_WIDTH/2 - MeasureTextEx(gameFont, backText, 20, 2).x/2,
        SCREEN_HEIGHT - 100
    };
    DrawTextEx(gameFont, backText, (Vector2){backPos.x + 2, backPos.y + 2}, 20, 2, BLACK);
    DrawTextEx(gameFont, backText, backPos, 20, 2, WHITE);

    // Add more options drawing here
    // For example: volume sliders, control config, etc.
}