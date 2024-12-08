#include "./headers/Game.h"
#include <windowsx.h>
#include <ctime>

Game::Game() : m_hWnd(nullptr), m_hInstance(nullptr), m_isRunning(false), m_backgroundBitmap(NULL) {}

Game::~Game() {
    if (m_backgroundBitmap) DeleteObject(m_backgroundBitmap);
}

bool Game::Initialize(HINSTANCE hInstance, int nCmdShow) {
    m_hInstance = hInstance;
    
    if (!RegisterWindowClass()) {
        return false;
    }

    if (!CreateGameWindow(nCmdShow)) {
        return false;
    }

    // Load background bitmap
    m_backgroundBitmap = (HBITMAP)LoadImage(NULL, "./assets/background.bmp", 
        IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
    if (!m_backgroundBitmap) {
        MessageBox(m_hWnd, "Failed to load background image", "Error", MB_ICONERROR);
        return false;
    }

    return true;
}

bool Game::RegisterWindowClass() {
    WNDCLASSEX wc;
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = Game::StaticWndProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = m_hInstance;
    wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.lpszMenuName = NULL;
    wc.lpszClassName = "GameWindowClass";
    wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

    if (!RegisterClassEx(&wc)) {
        MessageBox(NULL, "Window Registration Failed!", "Error", MB_ICONEXCLAMATION | MB_OK);
        return false;
    }
    return true;
}

bool Game::CreateGameWindow(int nCmdShow) {
    m_hWnd = CreateWindowEx(
        WS_EX_CLIENTEDGE,
        "GameWindowClass",
        "Game Window",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT,
        WINDOW_WIDTH, WINDOW_HEIGHT,
        NULL, NULL,
        m_hInstance,
        this  // Pass the Game instance to WndProc
    );

    if (m_hWnd == NULL) {
        MessageBox(NULL, "Window Creation Failed!", "Error", MB_ICONEXCLAMATION | MB_OK);
        return false;
    }

    ShowWindow(m_hWnd, nCmdShow);
    UpdateWindow(m_hWnd);
    return true;
}

LRESULT CALLBACK Game::StaticWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    if (message == WM_CREATE) {
        // Store the Game instance pointer
        CREATESTRUCT* pCreate = reinterpret_cast<CREATESTRUCT*>(lParam);
        SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pCreate->lpCreateParams));
    }

    // Get the Game instance pointer
    Game* game = reinterpret_cast<Game*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
    
    if (game) {
        return game->WndProc(hWnd, message, wParam, lParam);
    }

    return DefWindowProc(hWnd, message, wParam, lParam);
}

LRESULT CALLBACK Game::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
        case WM_CREATE: {
            // Create sprite types (this will also create the initial instances)
            if (!m_SpriteManager.CreateSpriteType("monster", 0, 0, RGB(181, 230, 29), 1, 4, true)) {
                MessageBox(hWnd, "Failed to create monster sprite type", "Error", MB_ICONERROR);
            }
            if (!m_SpriteManager.CreateSpriteType("walker", 0, 150, RGB(0, 255, 0), 1, 4, false)) {
                MessageBox(hWnd, "Failed to create walker sprite type", "Error", MB_ICONERROR);
            }
            if (!m_SpriteManager.CreateSpriteType("box", 0, 0, RGB(255, 255, 255), 1, 1, false)) {
                MessageBox(hWnd, "Failed to create box sprite type", "Error", MB_ICONERROR);
            }
            if (!m_SpriteManager.CreateSpriteType("apple", 0, 0, RGB(255, 255, 255), 1, 1, false)) {
                MessageBox(hWnd, "Failed to create apple sprite type", "Error", MB_ICONERROR);
            }

            // Load bitmaps
            if (!m_SpriteManager.LoadSpriteBitmap("monster", "./assets/monster.bmp")) {
                MessageBox(hWnd, "Failed to load monster.bmp", "Error", MB_ICONERROR);
            }
            if (!m_SpriteManager.LoadSpriteBitmap("walker", "./assets/walker.bmp")) {
                MessageBox(hWnd, "Failed to load walker.bmp", "Error", MB_ICONERROR);
            }
            if (!m_SpriteManager.LoadSpriteBitmap("box", "./assets/box.bmp")) {
                MessageBox(hWnd, "Failed to load box.bmp", "Error", MB_ICONERROR);
            }
            if (!m_SpriteManager.LoadSpriteBitmap("apple", "./assets/apple.bmp")) {
                MessageBox(hWnd, "Failed to load apple.bmp", "Error", MB_ICONERROR);
            }

            // Add initial instances
            if (!m_SpriteManager.AddInstance("monster", 300, 100)) {
                MessageBox(hWnd, "Failed to add initial instances", "Error", MB_ICONERROR);
            }
            if (!m_SpriteManager.AddInstance("walker", 100, 100)) {
                MessageBox(hWnd, "Failed to add initial instances", "Error", MB_ICONERROR);
            }
            if (!m_SpriteManager.AddInstance("box", 400, 300)) {
                MessageBox(hWnd, "Failed to add box instance", "Error", MB_ICONERROR);
            }

            // Set up timer for monster patrol (100ms interval)
            SetTimer(hWnd, 1, 100, NULL);

            // Return 0 to continue window creation
            return 0;
        }

        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);

            // Create double buffer
            HDC hdcBuffer = CreateCompatibleDC(hdc);
            HBITMAP hbmpBuffer = CreateCompatibleBitmap(hdc, WINDOW_WIDTH, WINDOW_HEIGHT);
            HBITMAP hbmpOld = (HBITMAP)SelectObject(hdcBuffer, hbmpBuffer);

            // Draw background
            if (m_backgroundBitmap) {
                HDC hdcBg = CreateCompatibleDC(hdc);
                HBITMAP hbmpOldBg = (HBITMAP)SelectObject(hdcBg, m_backgroundBitmap);
                
                // Get background bitmap dimensions
                BITMAP bm;
                GetObject(m_backgroundBitmap, sizeof(BITMAP), &bm);
                
                // Scale the background to fill the window
                SetStretchBltMode(hdcBuffer, HALFTONE);
                StretchBlt(hdcBuffer, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT,
                            hdcBg, 0, 0, bm.bmWidth, bm.bmHeight, SRCCOPY);
                
                SelectObject(hdcBg, hbmpOldBg);
                DeleteDC(hdcBg);
            } else {
                // Fallback to white background if bitmap failed to load
                RECT rect;
                GetClientRect(hWnd, &rect);
                FillRect(hdcBuffer, &rect, (HBRUSH)GetStockObject(WHITE_BRUSH));
            }

            // Render all sprites
            m_SpriteManager.RenderAll(hdcBuffer);

            // Copy buffer to screen
            BitBlt(hdc, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, hdcBuffer, 0, 0, SRCCOPY);

            SelectObject(hdcBuffer, hbmpOld);
            DeleteObject(hbmpBuffer);
            DeleteDC(hdcBuffer);

            EndPaint(hWnd, &ps);
            return 0;
        }

        case WM_KEYDOWN: {
            switch (wParam) {
                case VK_LEFT:
                    m_SpriteManager.MoveWalker(-5, 0);
                    InvalidateRect(hWnd, NULL, FALSE);
                    break;
                case VK_RIGHT:
                    m_SpriteManager.MoveWalker(5, 0);
                    InvalidateRect(hWnd, NULL, FALSE);
                    break;
                case VK_UP:
                    m_SpriteManager.MoveWalker(0, -5);
                    InvalidateRect(hWnd, NULL, FALSE);
                    break;
                case VK_DOWN:
                    m_SpriteManager.MoveWalker(0, 5);
                    InvalidateRect(hWnd, NULL, FALSE);
                    break;
            }
            return 0;
        }

        case WM_TIMER: {
            if (wParam == 1) {  // Our monster patrol timer
                m_SpriteManager.MonsterPatrol(0, 0);  // dx and dy will be calculated inside MonsterPatrol
                InvalidateRect(hWnd, NULL, FALSE);
            }
            return 0;
        }

        case WM_DESTROY:
            KillTimer(hWnd, 1);  // Clean up the timer
            PostQuitMessage(0);
            m_isRunning = false;
            return 0;
    }
    return DefWindowProc(hWnd, message, wParam, lParam);
}

void Game::Run() {
    m_isRunning = true;
    MSG msg;

    while (m_isRunning) {
        while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);

            if (msg.message == WM_QUIT) {
                m_isRunning = false;
            }
        }

        Update();
        // Force a redraw
        InvalidateRect(m_hWnd, NULL, FALSE);
    }
}

void Game::Update() {
    m_SpriteManager.UpdateAll();
    Sleep(16);  // Add frame timing like in the original code
}
