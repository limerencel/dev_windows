#include <windows.h>
#include "SpriteManager.h"
#include <vector>

#define TIMER_ID 1
#define FRAME_RATE 60

const int WINDOW_WIDTH = 1600;
const int WINDOW_HEIGHT = 600;
const int LIMITATION = 10;

SpriteManager spriteManager;

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
        case WM_CREATE: {
            // Create sprite types
            spriteManager.CreateSpriteType("monster", 0, 0, RGB(0, 229, 0));
            spriteManager.CreateSpriteType("walker", 0, 150, RGB(0, 255, 0), 10, 4, true);

            // Load bitmaps
            if (!spriteManager.LoadSpriteBitmap("monster", "./monster.bmp")) {
                MessageBox(hWnd, "Failed to load monster.bmp", "Error", MB_ICONERROR);
                return -1;
            }
            if (!spriteManager.LoadSpriteBitmap("walker", "./walker.bmp")) {
                MessageBox(hWnd, "Failed to load walker.bmp", "Error", MB_ICONERROR);
                return -1;
            }

            SetTimer(hWnd, TIMER_ID, 1000 / FRAME_RATE, NULL);
            return 0;
        }

        case WM_TIMER: {
            if (wParam == TIMER_ID) {
                spriteManager.UpdateAll();
                InvalidateRect(hWnd, NULL, FALSE);
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
            spriteManager.RenderAll(hdcBuffer);

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
                    if (spriteManager.HasSpace("monster")) {
                        spriteManager.AddInstance("monster");
                        InvalidateRect(hWnd, NULL, FALSE);
                    }
                    break;
                case 'W':
                    if (spriteManager.HasSpace("walker")) {
                        spriteManager.AddInstance("walker");
                        InvalidateRect(hWnd, NULL, FALSE);
                    }
                    break;
            }
            return 0;
        }

        case WM_DESTROY:
            KillTimer(hWnd, TIMER_ID);
            PostQuitMessage(0);
            return 0;
    }
    return DefWindowProc(hWnd, message, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    WNDCLASS wc = {0};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = "MyWindowClass";
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    RegisterClass(&wc);

    HWND hWnd = CreateWindow("MyWindowClass", "Sprite Renderer", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, WINDOW_WIDTH, WINDOW_HEIGHT, NULL, NULL, hInstance, NULL);

    if (!hWnd) {
        MessageBox(NULL, "Window creation failed!", "Error", MB_ICONERROR);
        return 0;
    }

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    MSG msg = {0};
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return (int)msg.wParam;
}
