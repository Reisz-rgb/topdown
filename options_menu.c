#include "options_menu.h"
#include "resources.h"
#include "raymath.h"

extern Music bgm; // Assume you have a global or externally accessible music object

void ApplyWindowMode(WindowMode mode) {
    switch (mode) {
        case WINDOW_ORIGINAL:
            if (IsWindowFullscreen()) ToggleFullscreen();
            SetWindowSize(SCREEN_WIDTH, SCREEN_HEIGHT);
            break;
        case WINDOW_MAXIMIZED: {
            if (IsWindowFullscreen()) ToggleFullscreen();
            int monitor = GetCurrentMonitor();
            int width = GetMonitorWidth(monitor);
            int height = GetMonitorHeight(monitor);
            SetWindowSize(width, height);
            break;
        }
        case WINDOW_FULLSCREEN:
            if (!IsWindowFullscreen()) ToggleFullscreen();
            break;
    }
}

void InitOptionsMenu(OptionsMenuState* state) {
    state->musicVolume = 0.5f;
    state->sfxVolume = 0.5f;
    state->windowMode = WINDOW_ORIGINAL;
    state->selectedOption = 0;

    SetMusicVolume(bgm, state->musicVolume);
}

bool UpdateOptionsMenu(OptionsMenuState* state) {
    // Move selection
    if (IsKeyPressed(KEY_DOWN)) state->selectedOption = (state->selectedOption + 1) % 3;
    if (IsKeyPressed(KEY_UP))   state->selectedOption = (state->selectedOption + 2) % 3;

    // Adjust volumes
    if (state->selectedOption == 0 && (IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_RIGHT))) {
        state->musicVolume += (IsKeyPressed(KEY_RIGHT) ? 0.1f : -0.1f);
        state->musicVolume = Clamp(state->musicVolume, 0.0f, 1.0f);
        SetMusicVolume(bgm, state->musicVolume);
    }

    if (state->selectedOption == 1 && (IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_RIGHT))) {
        state->sfxVolume += (IsKeyPressed(KEY_RIGHT) ? 0.1f : -0.1f);
        state->sfxVolume = Clamp(state->sfxVolume, 0.0f, 1.0f);
        SetSoundVolume(selectSfx, state->sfxVolume);
    }

    // Toggle window mode
    if (state->selectedOption == 2 && IsKeyPressed(KEY_ENTER)) {
        state->windowMode = (state->windowMode + 1) % 3;
        ApplyWindowMode(state->windowMode);
    }

    // Return to previous menu
    if (IsKeyPressed(KEY_Z)) {
        if (selectSfx.frameCount > 0) PlaySound(selectSfx);
        return true;
    }

    return false;
}

void DrawOptionsMenu(OptionsMenuState* state) {
    // Get center
    Vector2 center = { GetScreenWidth() / 2.0f, GetScreenHeight() / 2.0f };

    // Semi-transparent black background box
    int boxWidth = 500;
    int boxHeight = 250;
    int boxX = center.x - boxWidth / 2;
    int boxY = center.y - boxHeight / 2;
    DrawRectangle(boxX, boxY, boxWidth, boxHeight, (Color){0, 0, 0, 180});

    // Title
    const char* title = "OPTIONS";
    Vector2 titleSize = MeasureTextEx(gameFont, title, 30, 2);
    DrawTextEx(gameFont, title, (Vector2){center.x - titleSize.x / 2 + 2, boxY + 20 + 2}, 30, 2, BLACK);
    DrawTextEx(gameFont, title, (Vector2){center.x - titleSize.x / 2, boxY + 20}, 30, 2, WHITE);

    // Options
    float optionFontSize = 20;
    float optionSpacing = 40;
    float yStart = boxY + 70;

    Color selColor = YELLOW;
    Color normColor = WHITE;

    // Music Volume
    const char* musicLabel = "Music Volume";
    DrawTextEx(gameFont, musicLabel, (Vector2){center.x - 180, yStart}, optionFontSize, 2, state->selectedOption == 0 ? selColor : normColor);
    DrawTextEx(gameFont, TextFormat("%.0f%%", state->musicVolume * 100), (Vector2){center.x + 100, yStart}, optionFontSize, 2, WHITE);

    // SFX Volume
    const char* sfxLabel = "SFX Volume";
    DrawTextEx(gameFont, sfxLabel, (Vector2){center.x - 180, yStart + optionSpacing}, optionFontSize, 2, state->selectedOption == 1 ? selColor : normColor);
    DrawTextEx(gameFont, TextFormat("%.0f%%", state->sfxVolume * 100), (Vector2){center.x + 100, yStart + optionSpacing}, optionFontSize, 2, WHITE);

    // Window Mode
    const char* winLabel = "Window Mode";
    const char* modeText = "Windowed";
    if (state->windowMode == WINDOW_MAXIMIZED) modeText = "Maximized";
    else if (state->windowMode == WINDOW_FULLSCREEN) modeText = "Fullscreen";

    DrawTextEx(gameFont, winLabel, (Vector2){center.x - 180, yStart + optionSpacing * 2}, optionFontSize, 2, state->selectedOption == 2 ? selColor : normColor);
    DrawTextEx(gameFont, modeText, (Vector2){center.x + 100, yStart + optionSpacing * 2}, optionFontSize, 2, WHITE);

    // Back instruction
    const char* backText = "Press Z to return";
    Vector2 backSize = MeasureTextEx(gameFont, backText, 18, 2);
    DrawTextEx(gameFont, backText, (Vector2){center.x - backSize.x / 2 + 2, boxY + boxHeight - 30 + 2}, 18, 2, BLACK);
    DrawTextEx(gameFont, backText, (Vector2){center.x - backSize.x / 2, boxY + boxHeight - 30}, 18, 2, WHITE);
}
