#include "CMySprite.h"
#include <windows.h>
#include <vector>
#include <iostream>
#include "Resource.h"


const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 600;
CMySprite* selectedSprite = nullptr;
HDC hdcBuffer = NULL;        // Add buffer DC
HBITMAP hbmBuffer = NULL;    // Add buffer bitmap
std::vector<CMySprite*> sprites;

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    static HDC hdc;
    switch (message) {
        case WM_CREATE: {
            RECT rect;
            GetClientRect(hWnd, &rect);
            int width = rect.right - rect.left;
            int height = rect.bottom - rect.top;

            hdc = GetDC(hWnd);

            // Create double buffer
            hdcBuffer = CreateCompatibleDC(hdc);
            hbmBuffer = CreateCompatibleBitmap(hdc, width, height);
            SelectObject(hdcBuffer, hbmBuffer);

            HINSTANCE hInstance = (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE);
            sprites.push_back(new CMySprite(hdc, 100, 100));
            sprites[0]->LoadBitmap(hInstance, IDB_BITMAP1);
            sprites[0]->SetColorKey(RGB(0, 229, 0));  // Set color key for transparency

            sprites.push_back(new CMySprite(hdc, 300, 300));
            sprites[1]->LoadBitmap(hInstance, IDB_BITMAP2);
            sprites[1]->SetColorKey(RGB(0, 255, 0));  // Set color key for transparency

            ReleaseDC(hWnd, hdc);
            return 0;
        }

        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);

            // Get the actual client area size
            RECT clientRect;
            GetClientRect(hWnd, &clientRect);
            int width = clientRect.right - clientRect.left;
            int height = clientRect.bottom - clientRect.top;

            // Clear the buffer with white background
            HBRUSH hBrush = CreateSolidBrush(RGB(255, 255, 255));
            FillRect(hdcBuffer, &clientRect, hBrush);
            DeleteObject(hBrush);

            for (auto& sprite : sprites) {
                sprite->Render(hdcBuffer);
            }

            // Copy buffer to screen, using the actual client area size
            BitBlt(hdc, 0, 0, width, height, hdcBuffer, 0, 0, SRCCOPY);

            EndPaint(hWnd, &ps);
            return 0;
        }
        case WM_SIZE: {
            int width = LOWORD(lParam);
		    int height = HIWORD(lParam);

            // Delete old buffer bitmap
            if (hbmBuffer) DeleteObject(hbmBuffer);

            // Create new buffer bitmap
            HDC hdc = GetDC(hWnd);
		    hbmBuffer = CreateCompatibleBitmap(hdc, width, height);
		    SelectObject(hdcBuffer, hbmBuffer);
		    ReleaseDC(hWnd, hdc);

            // Force to a repaint
            InvalidateRect(hWnd, NULL, TRUE);
            return 0;
	    }
        case WM_KEYDOWN: {
            if (wParam == 'R' && selectedSprite) {
                selectedSprite->rotate(15);
                InvalidateRect(hWnd, NULL, FALSE);
                OutputDebugString("R key pressed, InvalidateRect called\n");
            }
            return 0;
        }
        case WM_ERASEBKGND:
            return 1;
        case WM_DESTROY:
            for (auto& sprite : sprites) {
                delete sprite;
            }
            sprites.clear();
            if (hdcBuffer) DeleteDC(hdcBuffer);
            if (hbmBuffer) DeleteObject(hbmBuffer);
            PostQuitMessage(0);
            return 0;
    }
    return DefWindowProc(hWnd, message, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    WNDCLASS wc = {};
    wc.lpfnWndProc = WndProc;
    wc.lpszClassName = "MySprite";
    wc.hInstance = hInstance;

    RegisterClass(&wc);

    HWND hWnd = CreateWindowEx(
        0,
        wc.lpszClassName,
        "My Sprite",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        WINDOW_WIDTH,
        WINDOW_HEIGHT,
        NULL,
        NULL,
        hInstance,
        NULL
    );

    RECT clientRect;
    GetClientRect(hWnd, &clientRect);
    int clientWidth = clientRect.right - clientRect.left;
    int clientHeight = clientRect.bottom - clientRect.top;

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    MSG msg = {};
    bool smallerSpriteVisible = true;
	CMySprite* smallerSprite = (sprites[0]->getArea() < sprites[1]->getArea()) ? sprites[0] : sprites[1];
	CMySprite* largerSprite = (smallerSprite == sprites[0]) ? sprites[1] : sprites[0];
    while (true) {
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
            if (msg.message == WM_QUIT) break;
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else {
            // Game logic updates
            bool needsRedraw = false;

            if (GetAsyncKeyState(VK_LBUTTON)) {
                POINT cursorPos;
                GetCursorPos(&cursorPos);
                ScreenToClient(hWnd, &cursorPos);

                // Deselect all the sprites
                for (auto& sprite : sprites) {
                    sprite->setSelected(false);
                }

                // Check if any sprite is clicked
                for (auto& sprite : sprites) {
                    if (sprite->containsPoint(cursorPos.x, cursorPos.y)) {
                        sprite->setSelected(true);
                        selectedSprite = sprite;
                        needsRedraw = true;
                        OutputDebugString("Sprite selected\n");
                        break;
                    }
                }
            }

            if (selectedSprite) {
                bool scaled = false;
                if (GetAsyncKeyState('I') & 0x8000) {
                    selectedSprite->increaseScale();
                    scaled = true;
                }
                if (GetAsyncKeyState('U') & 0x8000) {
                    selectedSprite->decreaseScale();
                    scaled = true;
                }
                if (scaled) {
                    needsRedraw = true;
                    Sleep(50);  // Add a small delay to control scaling speed
                }
            }

            // For transparency
            if (GetAsyncKeyState('T') & 0x8000) {
                selectedSprite->setTransparency(true);
                needsRedraw = true;
            }
            if (GetAsyncKeyState('O') & 0x8000) {
                selectedSprite->setTransparency(false);
                needsRedraw = true;
            }

            // Movement for monster (WASD)
            int dx0 = 0, dy0 = 0;
            if (GetAsyncKeyState('D') & 0x8000) dx0 += 1;
            if (GetAsyncKeyState('A') & 0x8000) dx0 -= 1;
            if (GetAsyncKeyState('W') & 0x8000) dy0 -= 1;
            if (GetAsyncKeyState('S') & 0x8000) dy0 += 1;

            // Movement for walker (Arrow keys)
            int dx1 = 0, dy1 = 0;
            if (GetAsyncKeyState(VK_RIGHT) & 0x8000) dx1 += 1;
            if (GetAsyncKeyState(VK_LEFT) & 0x8000) dx1 -= 1;
            if (GetAsyncKeyState(VK_UP) & 0x8000) dy1 -= 1;
            if (GetAsyncKeyState(VK_DOWN) & 0x8000) dy1 += 1;

            if (dx0 != 0 || dy0 != 0) {
                sprites[0]->move(dx0, dy0);
                sprites[0]->constrainToScreen(clientWidth, clientHeight);
                needsRedraw = true;
            }

            if (dx1 != 0 || dy1 != 0) {
                sprites[1]->move(dx1, dy1);
                sprites[1]->constrainToScreen(clientWidth, clientHeight);
                needsRedraw = true;
            }

            // Determine which sprite is larger
            CMySprite* largerSprite = (sprites[0]->getArea() > sprites[1]->getArea()) ? sprites[0] : sprites[1];
            CMySprite* smallerSprite = (sprites[0]->getArea() <= sprites[1]->getArea()) ? sprites[0] : sprites[1];

            // Check for collision
            if (smallerSprite->isColliding(largerSprite)) {
                smallerSprite->setVisible(false);
                needsRedraw = true;
            }

            if (needsRedraw) {
                // Clear the buffer with white background
                HBRUSH hBrush = CreateSolidBrush(RGB(255, 255, 255));
                RECT rect;
                GetClientRect(GetActiveWindow(), &rect);
                FillRect(hdcBuffer, &rect, hBrush);
                DeleteObject(hBrush);

                // Render all visible sprites
                for (auto& sprite : sprites) {
                    if (sprite->isVisible()) {
                        sprite->Render(hdcBuffer);
                    }
                }

                // Copy buffer to screen
                HDC hdc = GetDC(hWnd);
                BitBlt(hdc, 0, 0, clientWidth, clientHeight, hdcBuffer, 0, 0, SRCCOPY);
                ReleaseDC(hWnd, hdc);
            }

            Sleep(1);
        }
    }

    return (int)msg.wParam;
}