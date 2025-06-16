#include "game_screen.h"
#include "resources.h"

void InitGameScreen(GameScreenState* state) {
    // Initialize game state here
}

bool UpdateGameScreen(GameScreenState* state) {
    if (IsKeyPressed(KEY_Z)) {
        if (selectSfx.frameCount > 0) PlaySound(selectSfx);
        return true;
    }

    // Add game update logic here
    return false;
}

void DrawGameScreen(GameScreenState* state) {
    // Draw title
    const char* title = "GAME SCREEN";
    Vector2 titlePos = {
        SCREEN_WIDTH/2 - MeasureTextEx(gameFont, title, 30, 2).x/2,
        40
    };
    DrawTextEx(gameFont, title, (Vector2){titlePos.x + 2, titlePos.y + 2}, 30, 2, BLACK);
    DrawTextEx(gameFont, title, titlePos, 30, 2, WHITE);

    // Draw back instruction
    const char* backText = "Press Z to return to menu";
    Vector2 backPos = {
        SCREEN_WIDTH/2 - MeasureTextEx(gameFont, backText, 20, 2).x/2,
        SCREEN_HEIGHT - 100
    };
    DrawTextEx(gameFont, backText, (Vector2){backPos.x + 2, backPos.y + 2}, 20, 2, BLACK);
    DrawTextEx(gameFont, backText, backPos, 20, 2, WHITE);

    // Add game drawing here
}