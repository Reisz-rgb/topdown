#include "raylib.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define MAX_LAYERS 4
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
    bool active;  // Flag to track if this layer is valid
} ParallaxLayer;

typedef struct {
    const char* text;
    Rectangle bounds;
    bool selected;
} MenuItem;

void DrawParallaxBackground(ParallaxLayer *layers, int count) {
    for (int i = count - 1; i >= 0; i--) {
        // Only draw active layers with valid textures
        if (layers[i].active && layers[i].texture.id > 0) {
            DrawTexture(layers[i].texture, layers[i].position.x, layers[i].position.y, WHITE);
            DrawTexture(layers[i].texture, layers[i].position.x + layers[i].texture.width, layers[i].position.y, WHITE);
        }
    }
}

void UpdateParallaxLayers(ParallaxLayer *layers, int count, float scrollingSpeed) {
    static float scrollingPosition = 0.0f;
    scrollingPosition += scrollingSpeed;
    
    for (int i = 0; i < count; i++) {
        // Only update active layers with valid textures
        if (layers[i].active && layers[i].texture.id > 0 && layers[i].texture.width > 0) {
            layers[i].position.x = -scrollingPosition * layers[i].scrollSpeed;
            
            // Safely wrap around the position
            while (layers[i].position.x <= -layers[i].texture.width) {
                layers[i].position.x += layers[i].texture.width;
            }
        }
    }
}

int main(void)
{
    SetTraceLogLevel(LOG_ALL);  

    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "TopDown RPG");
    InitAudioDevice();
    SetTargetFPS(60);

    GameState currentState = TITLE_SCREEN;
    bool fadeOut = false;
    float alpha = 0.0f;
    bool shouldExit = false;
    bool musicLoaded = false;

    Font font = GetFontDefault();
    if (FileExists("resources/RobotoCondensed-Bold.ttf")) {
        Font loadedFont = LoadFont("resources/RobotoCondensed-Bold.ttf");
        if (loadedFont.texture.id > 0) {
            font = loadedFont;
        }
    }

    Music bgm = { 0 };
    if (FileExists("resources/Sound/background_music.ogg")) {
        bgm = LoadMusicStream("resources/Sound/background_music.ogg");
        if (bgm.ctxData != NULL) {
            musicLoaded = true;
            SetMusicVolume(bgm, 2.0f);  
            PlayMusicStream(bgm);
        }
    }

    Sound selectSfx = { 0 };
    Sound confirmSfx = { 0 };

    
    if (FileExists("resources/Sound/select.ogg")) {
        selectSfx = LoadSound("resources/Sound/select.ogg");
    }

    if (FileExists("resources/Sound/confirm.ogg")) {
        confirmSfx = LoadSound("resources/Sound/confirm.ogg");
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

    // Initialize all layers to inactive
    ParallaxLayer layers[MAX_LAYERS] = { 0 };
    int validLayers = 0;
    
    // Load background layers
    for (int i = 0; i < MAX_LAYERS; i++) {
        char filename[100];
        sprintf(filename, "resources/Image/Background/layer_%d.png", i + 1);
        
        layers[i].active = false;  // Default to inactive
        
        // Try to load the texture
        if (FileExists(filename)) {
            layers[i].texture = LoadTexture(filename);
            
            // Verify the texture loaded correctly
            if (layers[i].texture.id > 0 && layers[i].texture.width > 0) {
                layers[i].active = true;
                layers[i].position = (Vector2){ 0, 0 };
                layers[i].scrollSpeed = 0.1f * (i + 1);  // Foreground moves faster
                validLayers++;
                TraceLog(LOG_INFO, "Loaded texture for layer %d: %s", i, filename);
            } else {
                TraceLog(LOG_WARNING, "Failed to load texture: %s", filename);
            }
        } else {
            TraceLog(LOG_INFO, "Layer file does not exist: %s", filename);
        }
        
        // If loading failed, create a placeholder texture
        if (!layers[i].active) {
            Image placeholder = GenImageColor(SCREEN_WIDTH, SCREEN_HEIGHT,
                ColorAlpha((Color){ rand() % 200 + 55, rand() % 200 + 55, rand() % 200 + 55, 255 },
                (float)(i + 1) / MAX_LAYERS));
            
            if (placeholder.data != NULL) {
                layers[i].texture = LoadTextureFromImage(placeholder);
                UnloadImage(placeholder);
                
                if (layers[i].texture.id > 0 && layers[i].texture.width > 0) {
                    layers[i].active = true;
                    layers[i].position = (Vector2){ 0, 0 };
                    layers[i].scrollSpeed = 0.1f * (i + 1);
                    validLayers++;
                    TraceLog(LOG_INFO, "Created placeholder texture for layer %d", i);
                } else {
                    TraceLog(LOG_ERROR, "Failed to create placeholder texture for layer %d", i);
                }
            } else {
                TraceLog(LOG_ERROR, "Failed to generate placeholder image for layer %d", i);
            }
        }
    }
    
    // If no valid layers were loaded, create at least one default layer to avoid crashes
    if (validLayers == 0) {
        TraceLog(LOG_WARNING, "No valid layers loaded, creating default layer");
        Image defaultImage = GenImageColor(SCREEN_WIDTH, SCREEN_HEIGHT, DARKBLUE);
        if (defaultImage.data != NULL) {
            layers[0].texture = LoadTextureFromImage(defaultImage);
            UnloadImage(defaultImage);
            
            if (layers[0].texture.id > 0) {
                layers[0].active = true;
                layers[0].position = (Vector2){ 0, 0 };
                layers[0].scrollSpeed = 0.1f;
                validLayers = 1;
            }
        }
    }
    
    TraceLog(LOG_INFO, "Loaded %d valid parallax layers", validLayers);
    
    // Main game loop
    while (!shouldExit && !WindowShouldClose())
    {
        // Update music if loaded
        if (musicLoaded) {
            UpdateMusicStream(bgm);
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

        // Only update parallax if we have valid layers
        if (validLayers > 0) {
            float scrollSpeed = (currentState == TITLE_SCREEN) ? 0.5f : 1.0f;
            UpdateParallaxLayers(layers, MAX_LAYERS, scrollSpeed);
        }

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
                if (IsKeyPressed(KEY_Z)) {
                    if (selectSfx.stream.buffer && selectSfx.frameCount > 0) {
                        PlaySound(selectSfx);
                    }
                    currentState = MAIN_MENU;
                }
                break;

            case GAME_SCREEN:
                if (IsKeyPressed(KEY_Z)) {
                    if (selectSfx.stream.buffer && selectSfx.frameCount > 0) {
                        PlaySound(selectSfx);
                    }
                    currentState = MAIN_MENU;
                }
                break;
        }

        BeginDrawing();
            ClearBackground(BLACK);
            
            // Only draw parallax if we have valid layers
            if (validLayers > 0) {
                DrawParallaxBackground(layers, MAX_LAYERS);
            }

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
                    DrawTextEx(font, "Press Z to return", (Vector2){ SCREEN_WIDTH/2 - MeasureTextEx(font, "Press Z to return", 20, 2).x/2 + 2, SCREEN_HEIGHT - 100 + 2 }, 20, 2, BLACK);
                    DrawTextEx(font, "Press Z to return", (Vector2){ SCREEN_WIDTH/2 - MeasureTextEx(font, "Press Z to return", 20, 2).x/2, SCREEN_HEIGHT - 100 }, 20, 2, WHITE);
                    break;

                case GAME_SCREEN:
                    DrawTextEx(font, "GAME SCREEN", (Vector2){ SCREEN_WIDTH/2 - MeasureTextEx(font, "GAME SCREEN", 30, 2).x/2 + 2, 40 + 2 }, 30, 2, BLACK);
                    DrawTextEx(font, "GAME SCREEN", (Vector2){ SCREEN_WIDTH/2 - MeasureTextEx(font, "GAME SCREEN", 30, 2).x/2, 40 }, 30, 2, WHITE);
                    DrawTextEx(font, "Press Z to return to menu", (Vector2){ SCREEN_WIDTH/2 - MeasureTextEx(font, "Press Z to return to menu", 20, 2).x/2 + 2, SCREEN_HEIGHT - 100 + 2 }, 20, 2, BLACK);
                    DrawTextEx(font, "Press Z to return to menu", (Vector2){ SCREEN_WIDTH/2 - MeasureTextEx(font, "Press Z to return to menu", 20, 2).x/2, SCREEN_HEIGHT - 100 }, 20, 2, WHITE);
                    break;
            }

            if (fadeOut) {
                DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, (Color){ 0, 0, 0, (unsigned char)(alpha * 255) });
            }

        EndDrawing();
    }

    // Clean up resources - only do this once
    TraceLog(LOG_DEBUG, "Unloading resources...");
    
    // Unload all active layer textures
    for (int i = 0; i < MAX_LAYERS; i++) {
        if (layers[i].active && layers[i].texture.id > 0) {
            UnloadTexture(layers[i].texture);
            layers[i].active = false;
        }
    }

    // Unload font only if it's not the default font
    if (font.texture.id > 0 && font.texture.id != GetFontDefault().texture.id) {
        UnloadFont(font);
    }

    // Unload audio resources
    if (musicLoaded) UnloadMusicStream(bgm);
    if (selectSfx.frameCount > 0) UnloadSound(selectSfx);
    if (confirmSfx.frameCount > 0) UnloadSound(confirmSfx);

    CloseAudioDevice();
    CloseWindow();
    return 0;
}