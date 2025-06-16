#include <stddef.h>  
#include "raylib.h"
#include "resources.h"
#include "title_screen.h"
#include "main_menu.h"
#include "options_menu.h"
#include "game_screen.h"

typedef enum {
    TITLE_SCREEN,
    MAIN_MENU,
    OPTIONS_MENU,
    GAME_SCREEN
} GameState;

int main(void) {
    SetTraceLogLevel(LOG_ALL);  
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "TopDown RPG");
    InitAudioDevice();
    SetTargetFPS(60);

    LoadResources();

    GameState currentState = TITLE_SCREEN;
    bool shouldExit = false;

    // Initialize all screens
    TitleScreenState titleState;
    InitTitleScreen(&titleState);
    
    MainMenuState menuState;
    InitMainMenu(&menuState);
    
    OptionsMenuState optionsState;
    InitOptionsMenu(&optionsState);
    
    GameScreenState gameState;
    InitGameScreen(&gameState);

    while (!shouldExit && !WindowShouldClose()) {
        if (bgm.ctxData != NULL) {
            UpdateMusicStream(bgm);
        }

        switch (currentState) {
            case TITLE_SCREEN: {
                bool shouldStartGame = false;
                UpdateTitleScreen(&titleState, &shouldStartGame);
                if (shouldStartGame) {
                    currentState = MAIN_MENU;
                }
            } break;
            
            case MAIN_MENU: {
                MenuResult result = UpdateMainMenu(&menuState);
                switch (result) {
                    case MENU_RESULT_START_GAME: currentState = GAME_SCREEN; break;
                    case MENU_RESULT_OPTIONS: currentState = OPTIONS_MENU; break;
                    case MENU_RESULT_EXIT: shouldExit = true; break;
                    default: break;
                }
            } break;
            
            case OPTIONS_MENU: {
                if (UpdateOptionsMenu(&optionsState)) {
                    currentState = MAIN_MENU;
                }
            } break;
            
            case GAME_SCREEN: {
                if (UpdateGameScreen(&gameState)) {
                    currentState = MAIN_MENU;
                }
            } break;
        }

        float scrollSpeed = (currentState == TITLE_SCREEN) ? 0.5f : 1.0f;
        UpdateParallaxLayers(scrollSpeed);

        BeginDrawing();
            ClearBackground(BLACK);
            
            DrawParallaxBackground();

            switch (currentState) {
                case TITLE_SCREEN: DrawTitleScreen(&titleState); break;
                case MAIN_MENU: DrawMainMenu(&menuState); break;
                case OPTIONS_MENU: DrawOptionsMenu(&optionsState); break;
                case GAME_SCREEN: DrawGameScreen(&gameState); break;
            }
        EndDrawing();
    }

    UnloadResources();
    CloseAudioDevice();
    CloseWindow();
    return 0;
}