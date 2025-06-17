#include "options_menu.h"
#include "resources.h"
#include <math.h>

void InitOptionsMenu(OptionsMenuState* state) {
    state->musicVolume = 1.0f;    // Default to full volume
    state->sfxVolume = 1.0f;      // Default to full volume
    state->windowMode = 0;        // Default to original size
    state->selectedOption = 0;    // Start with first option selected
    state->settingsChanged = false;
}

bool UpdateOptionsMenu(OptionsMenuState* state) {
    // Handle navigation
    if (IsKeyPressed(KEY_DOWN)) {
        state->selectedOption = (state->selectedOption + 1) % 4;
        if (selectSfx.frameCount > 0) PlaySound(selectSfx);
    }
    else if (IsKeyPressed(KEY_UP)) {
        state->selectedOption = (state->selectedOption - 1 + 4) % 4;
        if (selectSfx.frameCount > 0) PlaySound(selectSfx);
    }

    // Handle option adjustments
    if (IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_RIGHT)) {
        state->settingsChanged = true;
        
        switch (state->selectedOption) {
            case 0: // Music Volume
                if (IsKeyPressed(KEY_LEFT)) state->musicVolume = fmaxf(0.0f, state->musicVolume - 0.1f);
                else state->musicVolume = fminf(1.0f, state->musicVolume + 0.1f);
                SetMusicVolume(bgm, state->musicVolume);
                break;
            
            case 1: // SFX Volume
                if (IsKeyPressed(KEY_LEFT)) state->sfxVolume = fmaxf(0.0f, state->sfxVolume - 0.1f);
                else state->sfxVolume = fminf(1.0f, state->sfxVolume + 0.1f);
                SetSoundVolume(selectSfx, state->sfxVolume);
                SetSoundVolume(confirmSfx, state->sfxVolume);
                break;
            
            case 2: // Window Size
                if (IsKeyPressed(KEY_LEFT)) state->windowMode = (state->windowMode - 1 + 3) % 3;
                else state->windowMode = (state->windowMode + 1) % 3;
                
                switch (state->windowMode) {
                    case 0: // Original size
                        ClearWindowState(FLAG_WINDOW_RESIZABLE);
                        SetWindowSize(SCREEN_WIDTH, SCREEN_HEIGHT);
                        ToggleFullscreen();
                        break;
                    case 1: // Maximized
                        ClearWindowState(FLAG_WINDOW_RESIZABLE);
                        MaximizeWindow();
                        break;
                    case 2: // Fullscreen
                        ToggleFullscreen();
                        break;
                }
                break;
        }
        if (selectSfx.frameCount > 0) PlaySound(selectSfx);
    }

    if (IsKeyPressed(KEY_Z)) {
        if (selectSfx.frameCount > 0) PlaySound(confirmSfx);
        return true;
    }
    return false;
}

void DrawOptionsMenu(OptionsMenuState* state) {
    // Draw background
    ClearBackground(BLACK);
    DrawParallaxBackground();

    // Draw semi-transparent overlay
    DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, (Color){0, 0, 0, 180});

    // Draw title
    const char* title = "OPTIONS";
    Vector2 titlePos = {
        SCREEN_WIDTH/2 - MeasureTextEx(gameFont, title, 40, 2).x/2,
        60
    };
    DrawTextEx(gameFont, title, (Vector2){titlePos.x + 3, titlePos.y + 3}, 40, 2, BLACK);
    DrawTextEx(gameFont, title, titlePos, 40, 2, WHITE);

    // Calculate positions for options
    float startY = 180;
    float optionSpacing = 50;
    
    // Draw music volume option
    Color musicColor = state->selectedOption == 0 ? YELLOW : WHITE;
    const char* musicText = TextFormat("MUSIC VOLUME: %.0f%%", state->musicVolume * 100);
    Vector2 musicPos = {SCREEN_WIDTH/2 - MeasureTextEx(gameFont, musicText, 24, 1).x/2, startY};
    DrawTextEx(gameFont, musicText, musicPos, 24, 1, musicColor);
    
    // Draw volume bar
    Rectangle musicBar = {SCREEN_WIDTH/2 - 100, startY + 35, 200, 20};
    DrawRectangleRec(musicBar, GRAY);
    DrawRectangleRec((Rectangle){musicBar.x, musicBar.y, musicBar.width * state->musicVolume, musicBar.height}, GREEN);
    DrawRectangleLinesEx(musicBar, 2, DARKGRAY);

    // Draw SFX volume option
    Color sfxColor = state->selectedOption == 1 ? YELLOW : WHITE;
    const char* sfxText = TextFormat("SFX VOLUME: %.0f%%", state->sfxVolume * 100);
    Vector2 sfxPos = {SCREEN_WIDTH/2 - MeasureTextEx(gameFont, sfxText, 24, 1).x/2, startY + optionSpacing};
    DrawTextEx(gameFont, sfxText, sfxPos, 24, 1, sfxColor);
    
    // Draw volume bar
    Rectangle sfxBar = {SCREEN_WIDTH/2 - 100, startY + optionSpacing + 35, 200, 20};
    DrawRectangleRec(sfxBar, GRAY);
    DrawRectangleRec((Rectangle){sfxBar.x, sfxBar.y, sfxBar.width * state->sfxVolume, sfxBar.height}, BLUE);
    DrawRectangleLinesEx(sfxBar, 2, DARKGRAY);

    // Draw window size option
    Color windowColor = state->selectedOption == 2 ? YELLOW : WHITE;
    const char* windowText = "WINDOW: ORIGINAL SIZE"; // Default value
    switch (state->windowMode) {
        case 0: windowText = "WINDOW: ORIGINAL SIZE"; break;
        case 1: windowText = "WINDOW: FULLSCREEN"; break;
    }
    Vector2 windowPos = {SCREEN_WIDTH/2 - MeasureTextEx(gameFont, windowText, 24, 1).x/2, startY + optionSpacing*2};
    DrawTextEx(gameFont, windowText, windowPos, 24, 1, windowColor);

    // Draw back option
    Color backColor = state->selectedOption == 3 ? YELLOW : WHITE;
    const char* backText = "BACK TO MENU";
    Vector2 backPos = {SCREEN_WIDTH/2 - MeasureTextEx(gameFont, backText, 24, 1).x/2, startY + optionSpacing*3};
    DrawTextEx(gameFont, backText, backPos, 24, 1, backColor);

    // Draw instructions
    const char* instructions = "USE ARROWS TO NAVIGATE AND ADJUST, Z TO BACK TO MAIN MENU";
    Vector2 instructionsPos = {
        SCREEN_WIDTH/2 - MeasureTextEx(gameFont, instructions, 18, 1).x/2,
        SCREEN_HEIGHT - 60
    };
    DrawTextEx(gameFont, instructions, instructionsPos, 18, 1, WHITE);

    // Draw indicator for changed settings
    if (state->settingsChanged) {
        const char* saveText = "CHANGES WILL BE SAVED AUTOMATICALLY";
        Vector2 savePos = {
            SCREEN_WIDTH/2 - MeasureTextEx(gameFont, saveText, 16, 1).x/2,
            SCREEN_HEIGHT - 30
        };
        DrawTextEx(gameFont, saveText, savePos, 16, 1, GREEN);
    }
}