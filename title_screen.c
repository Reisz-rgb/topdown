#include "title_screen.h"
#include "resources.h"

void InitTitleScreen(TitleScreenState* state) {
    state->blinkTimer = 0.0f;
    state->alpha = 0.0f;
    state->fadeOut = false;
}

void UpdateTitleScreen(TitleScreenState* state, bool* shouldStartGame) {
    state->blinkTimer += GetFrameTime();
    if (state->blinkTimer > 1.0f) state->blinkTimer = 0.0f;

    if (state->fadeOut) {
        state->alpha += 0.05f;
        if (state->alpha >= 1.0f) {
            state->alpha = 0.0f;
            state->fadeOut = false;
            *shouldStartGame = true;
        }
    }

    if (IsKeyPressed(KEY_ENTER)) {
        if (confirmSfx.frameCount > 0) {
            PlaySound(confirmSfx);
        }
        state->fadeOut = true;
    }
}

void DrawTitleScreen(TitleScreenState* state) {
    const char* title = "PARALLAX BACKGROUND";
    const char* pressEnterText = "PRESS ENTER TO CONTINUE";
    
    Vector2 titlePosition = { 
        SCREEN_WIDTH/2 - MeasureTextEx(gameFont, title, 40, 2).x/2,
        30
    };
    
    Vector2 pressEnterPosition = {
        SCREEN_WIDTH/2 - MeasureTextEx(gameFont, pressEnterText, 20, 2).x/2,
        SCREEN_HEIGHT - 100
    };

    DrawTextEx(gameFont, title, (Vector2){ titlePosition.x + 2, titlePosition.y + 2 }, 40, 2, BLACK);
    DrawTextEx(gameFont, title, titlePosition, 40, 2, WHITE);
    
    if (state->blinkTimer < 0.7f) {
        DrawTextEx(gameFont, pressEnterText, (Vector2){ pressEnterPosition.x + 2, pressEnterPosition.y + 2 }, 20, 2, BLACK);
        DrawTextEx(gameFont, pressEnterText, pressEnterPosition, 20, 2, WHITE);
    }

    if (state->fadeOut) {
        DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, (Color){ 0, 0, 0, (unsigned char)(state->alpha * 255) });
    }
}