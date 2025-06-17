#ifndef RESOURCES_H
#define RESOURCES_H

#include <stddef.h>  
#include "raylib.h"

#define MAX_LAYERS 4
#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

typedef struct {
    Texture2D texture;
    Vector2 position;
    float scrollSpeed;
    bool active;
} ParallaxLayer;

// Global game resources
extern Font gameFont;
extern Music bgm;
extern Sound selectSfx;
extern Sound confirmSfx;
extern ParallaxLayer layers[MAX_LAYERS];
extern int validLayers;

// Functions
void LoadResources();
void UnloadResources();
void UpdateParallaxLayers(float scrollingSpeed);
void DrawParallaxBackground();

#endif