#include "raylib.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define MAX_LAYERS 8
#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

typedef enum {
    TITLE_SCREEN,
    MAIN_MENU,
    OPTIONS_MENU,
    GAME_SCREEN
} GameState;

typedef struct {
    Texture2D texture;
    Vector2 position;
    float scrollSpeed;
} ParallaxLayer;

typedef struct {
    const char* text;
    Rectangle bounds;
    bool selected;
} MenuItem;

void DrawParallaxBackground(ParallaxLayer *layers, int count) {
    for (int i = count - 1; i >= 0; i--) {
        DrawTexture(layers[i].texture, layers[i].position.x, layers[i].position.y, WHITE);
        DrawTexture(layers[i].texture, layers[i].position.x + layers[i].texture.width, layers[i].position.y, WHITE);
    }
}

void UpdateParallaxLayers(ParallaxLayer *layers, int count, float scrollingSpeed) {
    static float scrollingPosition = 0.0f;
    scrollingPosition += scrollingSpeed;
    
    for (int i = 0; i < count; i++) {
        layers[i].position.x = -scrollingPosition * layers[i].scrollSpeed;
        while (layers[i].position.x <= -layers[i].texture.width) {
            layers[i].position.x += layers[i].texture.width;
        }
    }
}

int main(void)
{
    SetTraceLogLevel(LOG_ALL);  // Enable all logs

    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Parallax Background Demo");
    InitAudioDevice();
    SetTargetFPS(60);

    GameState currentState = TITLE_SCREEN;
    bool fadeOut = false;
    float alpha = 0.0f;
    bool shouldExit = false;
    bool musicLoaded = false;

    // Modified font loading and unloading
    Font font = GetFontDefault();  // Start with default font
    if (FileExists("resources/RobotoCondensed-Bold.ttf")) {
        // Only assign a new font if the file exists
        Font loadedFont = LoadFont("resources/RobotoCondensed-Bold.ttf");
        if (loadedFont.texture.id > 0) { // Check if successfully loaded
            font = loadedFont;
        }
    }

    Music bgm = { 0 };
    if (FileExists("resources/background_music.mp3")) {
        bgm = LoadMusicStream("resources/background_music.mp3");
        if (bgm.ctxData != NULL) {
            musicLoaded = true;
            SetMusicVolume(bgm, 0.7f);
            PlayMusicStream(bgm);
        }
    }

    Sound selectSfx = { 0 };
    Sound confirmSfx = { 0 };

    if (FileExists("resources/select.wav")) {
        selectSfx = LoadSound("resources/select.wav");
    }

    if (FileExists("resources/confirm.wav")) {
        confirmSfx = LoadSound("resources/confirm.wav");
    }

    const char* title = "PARALLAX BACKGROUND";
    const char* pressEnterText = "PRESS ENTER TO CONTINUE";

    Vector2 titlePosition = { 0 };
    Vector2 pressEnterPosition = { 0 };

    if (font.texture.id > 0) {
        titlePosition = (Vector2){ 
            SCREEN_WIDTH/2 - MeasureTextEx(font, title, 40, 2).x/2,
            30
        };
        pressEnterPosition = (Vector2){
            SCREEN_WIDTH/2 - MeasureTextEx(font, pressEnterText, 20, 2).x/2,
            SCREEN_HEIGHT - 100
        };
    }

    MenuItem mainMenuItems[3] = {
        { "START GAME", { 0, 0, 0, 0 }, true },
        { "OPTIONS", { 0, 0, 0, 0 }, false },
        { "EXIT", { 0, 0, 0, 0 }, false }
    };

    for (int i = 0; i < 3; i++) {
        Vector2 textSize = MeasureTextEx(font, mainMenuItems[i].text, 30, 2);
        mainMenuItems[i].bounds = (Rectangle) {
            SCREEN_WIDTH/2 - textSize.x/2,
            SCREEN_HEIGHT/2 - 60 + i * 80,
            textSize.x,
            textSize.y
        };
    }

    int selectedMenuItem = 0;
    float menuBlinkTimer = 0.0f;

    ParallaxLayer layers[MAX_LAYERS];
    for (int i = 0; i < MAX_LAYERS; i++) {
        char filename[100];
        sprintf(filename, "resources/layer_%d.png", i + 1);
        layers[i].texture = LoadTexture(filename);

        if (layers[i].texture.id == 0) {
            Image placeholder = GenImageColor(SCREEN_WIDTH, SCREEN_HEIGHT,
                ColorAlpha((Color){ rand() % 200 + 55, rand() % 200 + 55, rand() % 200 + 55, 255 },
                (float)(MAX_LAYERS - i) / MAX_LAYERS));
            layers[i].texture = LoadTextureFromImage(placeholder);
            UnloadImage(placeholder);
        }

        if (layers[i].texture.id == 0) {
            TraceLog(LOG_ERROR, "Failed to load texture for layer %d", i);
            CloseWindow();
            return 1;
        }

        layers[i].position = (Vector2){ 0, 0 };
        layers[i].scrollSpeed = 0.1f * (MAX_LAYERS - i);
    }

    while (!shouldExit && !WindowShouldClose())
    {
        if (bgm.ctxData != NULL) {
            StopMusicStream(bgm);  // Add this to ensure music is stopped before unloading
            UnloadMusicStream(bgm);
        }

        menuBlinkTimer += GetFrameTime();
        if (menuBlinkTimer > 1.0f) menuBlinkTimer = 0.0f;

        if (fadeOut) {
            alpha += 0.05f;
            if (alpha >= 1.0f) {
                alpha = 0.0f;
                fadeOut = false;
            }
        }

        float scrollSpeed = (currentState == TITLE_SCREEN) ? 0.5f : 1.0f;
        UpdateParallaxLayers(layers, MAX_LAYERS, scrollSpeed);

        switch (currentState) {
            case TITLE_SCREEN:
                if (IsKeyPressed(KEY_ENTER)) {
                    if (confirmSfx.stream.buffer && confirmSfx.frameCount > 0) {
                        PlaySound(confirmSfx);
                    }
                    currentState = MAIN_MENU;
                }
                break;

            case MAIN_MENU:
                if (IsKeyPressed(KEY_DOWN)) {
                    if (selectSfx.stream.buffer && selectSfx.frameCount > 0) {
                        PlaySound(selectSfx);
                    }
                    selectedMenuItem = (selectedMenuItem + 1) % 3;
                    for (int i = 0; i < 3; i++) {
                        mainMenuItems[i].selected = (i == selectedMenuItem);
                    }
                }

                if (IsKeyPressed(KEY_UP)) {
                    if (selectSfx.stream.buffer && selectSfx.frameCount > 0) {
                        PlaySound(selectSfx);
                    }
                    selectedMenuItem = (selectedMenuItem - 1 + 3) % 3;
                    for (int i = 0; i < 3; i++) {
                        mainMenuItems[i].selected = (i == selectedMenuItem);
                    }
                }

                if (IsKeyPressed(KEY_ENTER)) {
                    if (confirmSfx.stream.buffer && confirmSfx.frameCount > 0) {
                        PlaySound(confirmSfx);
                    }
                    switch (selectedMenuItem) {
                        case 0: currentState = GAME_SCREEN; break;
                        case 1: currentState = OPTIONS_MENU; break;
                        case 2: shouldExit = true; break;
                    }
                }
                break;

            case OPTIONS_MENU:
                if (IsKeyPressed(KEY_ESCAPE) || IsKeyPressed(KEY_ENTER)) {
                    if (selectSfx.stream.buffer && selectSfx.frameCount > 0) {
                        PlaySound(selectSfx);
                    }
                    currentState = MAIN_MENU;
                }
                break;

            case GAME_SCREEN:
                if (IsKeyPressed(KEY_ESCAPE)) {
                    if (selectSfx.stream.buffer && selectSfx.frameCount > 0) {
                        PlaySound(selectSfx);
                    }
                    currentState = MAIN_MENU;
                }
                break;
        }

        BeginDrawing();
            ClearBackground(BLACK);
            DrawParallaxBackground(layers, MAX_LAYERS);

            switch (currentState) {
                case TITLE_SCREEN:
                    DrawTextEx(font, title, (Vector2){ titlePosition.x + 2, titlePosition.y + 2 }, 40, 2, BLACK);
                    DrawTextEx(font, title, titlePosition, 40, 2, WHITE);
                    if (menuBlinkTimer < 0.7f) {
                        DrawTextEx(font, pressEnterText, (Vector2){ pressEnterPosition.x + 2, pressEnterPosition.y + 2 }, 20, 2, BLACK);
                        DrawTextEx(font, pressEnterText, pressEnterPosition, 20, 2, WHITE);
                    }
                    break;

                case MAIN_MENU:
                    DrawTextEx(font, "MAIN MENU", (Vector2){ SCREEN_WIDTH/2 - MeasureTextEx(font, "MAIN MENU", 30, 2).x/2 + 2, 40 + 2 }, 30, 2, BLACK);
                    DrawTextEx(font, "MAIN MENU", (Vector2){ SCREEN_WIDTH/2 - MeasureTextEx(font, "MAIN MENU", 30, 2).x/2, 40 }, 30, 2, WHITE);
                    for (int i = 0; i < 3; i++) {
                        Color textColor = mainMenuItems[i].selected ? (menuBlinkTimer < 0.7f ? YELLOW : GOLD) : WHITE;
                        DrawTextEx(font, mainMenuItems[i].text, (Vector2){ mainMenuItems[i].bounds.x + 2, mainMenuItems[i].bounds.y + 2 }, 30, 2, BLACK);
                        DrawTextEx(font, mainMenuItems[i].text, (Vector2){ mainMenuItems[i].bounds.x, mainMenuItems[i].bounds.y }, 30, 2, textColor);
                        if (mainMenuItems[i].selected) {
                            DrawTextEx(font, ">", (Vector2){ mainMenuItems[i].bounds.x - 40 + 2, mainMenuItems[i].bounds.y + 2 }, 30, 2, BLACK);
                            DrawTextEx(font, ">", (Vector2){ mainMenuItems[i].bounds.x - 40, mainMenuItems[i].bounds.y }, 30, 2, textColor);
                        }
                    }
                    break;

                case OPTIONS_MENU:
                    DrawTextEx(font, "OPTIONS", (Vector2){ SCREEN_WIDTH/2 - MeasureTextEx(font, "OPTIONS", 30, 2).x/2 + 2, 40 + 2 }, 30, 2, BLACK);
                    DrawTextEx(font, "OPTIONS", (Vector2){ SCREEN_WIDTH/2 - MeasureTextEx(font, "OPTIONS", 30, 2).x/2, 40 }, 30, 2, WHITE);
                    DrawTextEx(font, "Press ENTER or ESC to return", (Vector2){ SCREEN_WIDTH/2 - MeasureTextEx(font, "Press ENTER or ESC to return", 20, 2).x/2 + 2, SCREEN_HEIGHT - 100 + 2 }, 20, 2, BLACK);
                    DrawTextEx(font, "Press ENTER or ESC to return", (Vector2){ SCREEN_WIDTH/2 - MeasureTextEx(font, "Press ENTER or ESC to return", 20, 2).x/2, SCREEN_HEIGHT - 100 }, 20, 2, WHITE);
                    break;

                case GAME_SCREEN:
                    DrawTextEx(font, "GAME SCREEN", (Vector2){ SCREEN_WIDTH/2 - MeasureTextEx(font, "GAME SCREEN", 30, 2).x/2 + 2, 40 + 2 }, 30, 2, BLACK);
                    DrawTextEx(font, "GAME SCREEN", (Vector2){ SCREEN_WIDTH/2 - MeasureTextEx(font, "GAME SCREEN", 30, 2).x/2, 40 }, 30, 2, WHITE);
                    DrawTextEx(font, "Press ESC to return to menu", (Vector2){ SCREEN_WIDTH/2 - MeasureTextEx(font, "Press ESC to return to menu", 20, 2).x/2 + 2, SCREEN_HEIGHT - 100 + 2 }, 20, 2, BLACK);
                    DrawTextEx(font, "Press ESC to return to menu", (Vector2){ SCREEN_WIDTH/2 - MeasureTextEx(font, "Press ESC to return to menu", 20, 2).x/2, SCREEN_HEIGHT - 100 }, 20, 2, WHITE);
                    break;
            }

            if (fadeOut) {
                DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, (Color){ 0, 0, 0, (unsigned char)(alpha * 255) });
            }

        EndDrawing();
    }

    for (int i = 0; i < MAX_LAYERS; i++) {
        if (layers[i].texture.id > 0) UnloadTexture(layers[i].texture);
    }

    TraceLog(LOG_DEBUG, "Unloading resources...");
    TraceLog(LOG_DEBUG, "Font texture ID: %d, Default font texture ID: %d", 
         font.texture.id, GetFontDefault().texture.id);

    if (font.texture.id > 0 && font.texture.id != GetFontDefault().texture.id) {
        UnloadFont(font);
    }

    if (bgm.ctxData != NULL) {
        UnloadMusicStream(bgm);
    }

    if (selectSfx.stream.buffer && selectSfx.frameCount > 0) {
        UnloadSound(selectSfx);
    }

    if (confirmSfx.stream.buffer && confirmSfx.frameCount > 0) {
        UnloadSound(confirmSfx);
    }

    CloseAudioDevice();
    CloseWindow();

    return 0;
}