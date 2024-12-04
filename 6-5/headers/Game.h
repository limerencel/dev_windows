#pragma once
#include <windows.h>
#include "SpriteManager.h"
#include "WindowConfig.h"

class Game {
public:
    Game();
    ~Game();

    bool Initialize(HINSTANCE hInstance, int nCmdShow);
    void Run();
    
    // Static method to handle Windows messages
    static LRESULT CALLBACK StaticWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

private:
    // The actual window procedure that belongs to our game instance
    LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
    
    void Update();
    void Render();
    
    HWND m_hWnd;
    HINSTANCE m_hInstance;
    bool m_isRunning;
    SpriteManager m_SpriteManager;
    HBITMAP m_backgroundBitmap;

    // Helper method to register the window class
    bool RegisterWindowClass();
    // Helper method to create the game window
    bool CreateGameWindow(int nCmdShow);
};