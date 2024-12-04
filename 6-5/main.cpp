#include <windows.h>
#include "./headers/Game.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    Game game;
    if (game.Initialize(hInstance, nCmdShow)) {
        game.Run();
    }
    return 0;
}