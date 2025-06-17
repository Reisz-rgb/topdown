#include "resources.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

Font gameFont = { 0 };
Music bgm = { 0 };
Sound selectSfx = { 0 };
Sound confirmSfx = { 0 };
ParallaxLayer layers[MAX_LAYERS] = { 0 };
int validLayers = 0;

void LoadResources() {
    // Load font
    gameFont = GetFontDefault();
    if (FileExists("resources/RobotoCondensed-Bold.ttf")) {
        Font loadedFont = LoadFont("resources/RobotoCondensed-Bold.ttf");
        if (loadedFont.texture.id > 0) {
            gameFont = loadedFont;
        }
    }

    // Load music
    if (FileExists("resources/Sound/background_music.ogg")) {
        bgm = LoadMusicStream("resources/Sound/background_music.ogg");
        if (bgm.ctxData != NULL) {
            SetMusicVolume(bgm, 2.0f);
            PlayMusicStream(bgm);
        }
    }

    // Load sounds
    if (FileExists("resources/Sound/select.ogg")) {
        selectSfx = LoadSound("resources/Sound/select.ogg");
    }
    if (FileExists("resources/Sound/confirm.ogg")) {
        confirmSfx = LoadSound("resources/Sound/confirm.ogg");
    }

    // Load parallax layers
    validLayers = 0;
    for (int i = 0; i < MAX_LAYERS; i++) {
        char filename[100];
        sprintf(filename, "resources/Image/Background/layer_%d.png", i + 1);
        
        layers[i].active = false;
        
        if (FileExists(filename)) {
            layers[i].texture = LoadTexture(filename);
            
            if (layers[i].texture.id > 0 && layers[i].texture.width > 0) {
                layers[i].active = true;
                layers[i].position = (Vector2){ 0, 0 };
                layers[i].scrollSpeed = 0.1f * (i + 1);
                validLayers++;
            }
        }
        
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
                }
            }
        }
    }
    
    if (validLayers == 0) {
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
}

void UnloadResources() {
    for (int i = 0; i < MAX_LAYERS; i++) {
        if (layers[i].active && layers[i].texture.id > 0) {
            UnloadTexture(layers[i].texture);
            layers[i].active = false;
        }
    }

    if (gameFont.texture.id > 0 && gameFont.texture.id != GetFontDefault().texture.id) {
        UnloadFont(gameFont);
    }

    if (bgm.ctxData != NULL) UnloadMusicStream(bgm);
    if (selectSfx.frameCount > 0) UnloadSound(selectSfx);
    if (confirmSfx.frameCount > 0) UnloadSound(confirmSfx);
}

void UpdateParallaxLayers(float scrollingSpeed) {
    static double scrollingPosition = 0.0;
    scrollingPosition += scrollingSpeed;
    
    for (int i = 0; i < MAX_LAYERS; i++) {
        if (layers[i].active && layers[i].texture.id > 0) {
            float width = (float)layers[i].texture.width;
            
            // Smoother speed distribution
            float layerSpeed = 0.4f + (0.25f * i);
            layers[i].position.x = -(float)(scrollingPosition * layerSpeed);
            
            // Improved wrapping
            layers[i].position.x = fmodf(layers[i].position.x, width);
            if (layers[i].position.x > 0) layers[i].position.x -= width;
        }
    }
}

void DrawParallaxBackground() {
    for (int i = MAX_LAYERS - 1; i >= 0; i--) {
        if (layers[i].active && layers[i].texture.id > 0) {
            float width = (float)layers[i].texture.width;
            
            // Draw enough copies to cover any position
            for (int j = -1; j <= 1; j++) {
                DrawTexture(layers[i].texture, 
                          layers[i].position.x + (j * width), 
                          layers[i].position.y, 
                          WHITE);
            }
        }
    }
}