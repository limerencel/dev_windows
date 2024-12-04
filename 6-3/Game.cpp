#include "Game.h"
#include <windowsx.h>  // For GET_X_LPARAM and GET_Y_LPARAM
#include <ctime>  // For time function

Game::Game() : m_hWnd(nullptr), m_hInstance(nullptr), m_isRunning(false) {
    // Initialize random number generator
    srand(static_cast<unsigned>(time(nullptr)));
}

Game::~Game() {
}

bool Game::Initialize(HINSTANCE hInstance, int nCmdShow) {
    m_hInstance = hInstance;
    
    if (!RegisterWindowClass()) {
        return false;
    }

    if (!CreateGameWindow(nCmdShow)) {
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
            // Create sprite types
            m_SpriteManagerList.CreateSpriteType("monster", 0, 0, RGB(0, 229, 0));
            m_SpriteManagerList.CreateSpriteType("walker", 0, 150, RGB(0, 255, 0), 10, 4, true);

            // Load bitmaps
            if (!m_SpriteManagerList.LoadSpriteBitmap("monster", "./monster.bmp")) {
                MessageBox(hWnd, "Failed to load monster.bmp", "Error", MB_ICONERROR);
                return -1;
            }
            if (!m_SpriteManagerList.LoadSpriteBitmap("walker", "./walker.bmp")) {
                MessageBox(hWnd, "Failed to load walker.bmp", "Error", MB_ICONERROR);
                return -1;
            }
            return 0;
        }

        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);

            // Create double buffer
            HDC hdcBuffer = CreateCompatibleDC(hdc);
            HBITMAP hbmpBuffer = CreateCompatibleBitmap(hdc, WINDOW_WIDTH, WINDOW_HEIGHT);
            HBITMAP hbmpOld = (HBITMAP)SelectObject(hdcBuffer, hbmpBuffer);

            // Clear background
            RECT rect;
            GetClientRect(hWnd, &rect);
            FillRect(hdcBuffer, &rect, (HBRUSH)GetStockObject(WHITE_BRUSH));

            // Render all sprites
            m_SpriteManagerList.RenderAll(hdcBuffer);

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
                case 'M':
                    if (m_SpriteManagerList.HasSpace("monster")) {
                        m_SpriteManagerList.AddInstance("monster");
                        m_SpriteManagerList.SelectSpriteAt(WINDOW_WIDTH/2, WINDOW_HEIGHT/2);
                        InvalidateRect(hWnd, NULL, FALSE);
                    }
                    break;
                case 'W':
                    if (m_SpriteManagerList.HasSpace("walker")) {
                        m_SpriteManagerList.AddInstance("walker");
                        InvalidateRect(hWnd, NULL, FALSE);
                    }
                    break;
                case VK_LEFT:
                    m_SpriteManagerList.MoveSelectedSpriteBy(-5, 0);
                    InvalidateRect(hWnd, NULL, FALSE);
                    break;
                case VK_RIGHT:
                    m_SpriteManagerList.MoveSelectedSpriteBy(5, 0);
                    InvalidateRect(hWnd, NULL, FALSE);
                    break;
                case VK_UP:
                    m_SpriteManagerList.MoveSelectedSpriteBy(0, -5);
                    InvalidateRect(hWnd, NULL, FALSE);
                    break;
                case VK_DOWN:
                    m_SpriteManagerList.MoveSelectedSpriteBy(0, 5);
                    InvalidateRect(hWnd, NULL, FALSE);
                    break;
            }
            return 0;
        }

        case WM_LBUTTONDOWN: {
            int mouseX = GET_X_LPARAM(lParam);
            int mouseY = GET_Y_LPARAM(lParam);
            m_SpriteManagerList.SelectSpriteAt(mouseX, mouseY);
            InvalidateRect(hWnd, NULL, FALSE);
            return 0;
        }

        case WM_DESTROY:
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
    m_SpriteManagerList.UpdateAll();
    Sleep(16);  // Add frame timing like in the original code
}
