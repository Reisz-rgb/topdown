#include "main_menu.h"
#include "resources.h"

void InitMainMenu(MainMenuState* state) {
    state->selectedItem = 0;
    state->blinkTimer = 0.0f;
}

MenuResult UpdateMainMenu(MainMenuState* state) {
    state->blinkTimer += GetFrameTime();
    if (state->blinkTimer > 1.0f) state->blinkTimer = 0.0f;

    if (IsKeyPressed(KEY_DOWN)) {
        if (selectSfx.frameCount > 0) PlaySound(selectSfx);
        state->selectedItem = (state->selectedItem + 1) % 3;
    }

    if (IsKeyPressed(KEY_UP)) {
        if (selectSfx.frameCount > 0) PlaySound(selectSfx);
        state->selectedItem = (state->selectedItem - 1 + 3) % 3;
    }

    if (IsKeyPressed(KEY_ENTER)) {
        if (confirmSfx.frameCount > 0) PlaySound(confirmSfx);
        switch (state->selectedItem) {
            case 0: return MENU_RESULT_START_GAME;
            case 1: return MENU_RESULT_OPTIONS;
            case 2: return MENU_RESULT_EXIT;
        }
    }

    return MENU_RESULT_NONE;
}

void DrawMainMenu(MainMenuState* state) {
    const char* menuItems[] = {"START GAME", "OPTIONS", "EXIT"};
    
    // Draw menu title
    const char* title = "MAIN MENU";
    Vector2 titlePos = {
        SCREEN_WIDTH/2 - MeasureTextEx(gameFont, title, 30, 2).x/2,
        40
    };
    DrawTextEx(gameFont, title, (Vector2){titlePos.x + 2, titlePos.y + 2}, 30, 2, BLACK);
    DrawTextEx(gameFont, title, titlePos, 30, 2, WHITE);

    // Draw menu items
    for (int i = 0; i < 3; i++) {
        Vector2 textSize = MeasureTextEx(gameFont, menuItems[i], 30, 2);
        Vector2 position = {
            SCREEN_WIDTH/2 - textSize.x/2,
            SCREEN_HEIGHT/2 - 60 + i * 80
        };

        Color color = (i == state->selectedItem) ? 
            (state->blinkTimer < 0.7f ? YELLOW : GOLD) : WHITE;

        // Draw text shadow
        DrawTextEx(gameFont, menuItems[i], 
            (Vector2){position.x + 2, position.y + 2}, 
            30, 2, BLACK);

        // Draw main text
        DrawTextEx(gameFont, menuItems[i], position, 30, 2, color);

        // Draw selection indicator
        if (i == state->selectedItem) {
            DrawTextEx(gameFont, ">", 
                (Vector2){position.x - 40, position.y}, 
                30, 2, color);
        }
    }
}