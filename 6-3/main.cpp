#include "Game.h"

int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    Game game;
    
    if (!game.Initialize(hInstance, nCmdShow)) {
        return -1;
    }

    game.Run();
    return 0;
}