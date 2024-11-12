#include "CMySprite.h"
#include <windows.h>
#include <vector>
#include <iostream>

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
        hdc = GetDC(hWnd);
        
        // Create double buffer
        hdcBuffer = CreateCompatibleDC(hdc);
        hbmBuffer = CreateCompatibleBitmap(hdc, WINDOW_WIDTH, WINDOW_HEIGHT);
        SelectObject(hdcBuffer, hbmBuffer);
        
        sprites.push_back(new CMySprite(hdc, 100, 100));
        sprites[0]->LoadBitmap("./monster.bmp");
        sprites[0]->SetColorKey(RGB(0, 229, 0));  // Set color key for transparency
        
        sprites.push_back(new CMySprite(hdc, 300, 300));
        sprites[1]->LoadBitmap("./monster.bmp");
        sprites[1]->SetColorKey(RGB(0, 229, 0));  // Set color key for transparency
        ReleaseDC(hWnd, hdc);
        return 0;
      }

      case WM_PAINT: {
          PAINTSTRUCT ps;
          HDC hdc = BeginPaint(hWnd, &ps);
          
          // Clear the buffer with white background
          HBRUSH hBrush = CreateSolidBrush(RGB(255, 255, 255));  // or any background color
          RECT rect;
          GetClientRect(hWnd, &rect);
          FillRect(hdcBuffer, &rect, hBrush);
          DeleteObject(hBrush);
          
          for (auto& sprite : sprites) {
            sprite->Render(hdcBuffer);
          }
          
          // Copy buffer to screen
          BitBlt(hdc, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, hdcBuffer, 0, 0, SRCCOPY);
          
          EndPaint(hWnd, &ps);
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
  while (true) {
    if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
      if (msg.message == WM_QUIT) break;
      TranslateMessage(&msg);
      DispatchMessage(&msg);
    } else {
      // Game logic updates
      bool needsRedraw = false;

      if (GetAsyncKeyState(VK_LBUTTON)) {
        POINT cursorPos;
        GetCursorPos(&cursorPos);
        ScreenToClient(hWnd, &cursorPos);

        // Deselect all the sprits
        for (auto& sprite : sprites) {
          sprite->setSelected(false);
        }

        // Check if any sprite is clicked
        for (auto& sprite : sprites) {
          if (sprite->containsPoint(cursorPos.x, cursorPos.y)) {
            sprite->setSelected(true);
            selectedSprite = sprite;
            needsRedraw = true;
            break;
          }
        }

      }
        // Rotate selected sprite
      // if (selectedSprite && (GetAsyncKeyState('R') * 0x8000)) {
      //   selectedSprite->rotate(15.0f);
      //   InvalidateRect(hWnd, NULL, TRUE);
      // }
      if (GetAsyncKeyState('I') & 0x8000) {
        selectedSprite->increaseScale();
      }
      if (GetAsyncKeyState('D') & 0x8000) {
        selectedSprite->decreaseScale();
      }

      // For transparency
      if (GetAsyncKeyState('T') & 0x8000) {
        selectedSprite->setTransparency(true);
      }
      if (GetAsyncKeyState('O') & 0x8000) {
        selectedSprite->setTransparency(false);
      }

      // Movement
      int dx = 0, dy = 0;
      if (GetAsyncKeyState(VK_RIGHT) & 0x8000) {
        dx += 1;
      }
      if (GetAsyncKeyState(VK_LEFT) & 0x8000) {
        dx -= 1;
      }
      if (GetAsyncKeyState(VK_UP) & 0x8000) {
        dy -= 1;
      }
      if (GetAsyncKeyState(VK_DOWN) & 0x8000) {
        dy += 1;
      }

      if (dx!= 0 || dy!= 0) {
        selectedSprite->move(dx, dy);
        selectedSprite->constrainToScreen(clientWidth, clientHeight);
      }

      // Render
      // Clear the buffer with white background
      HBRUSH hBrush = CreateSolidBrush(RGB(255, 255, 255));
      RECT rect;
      GetClientRect(GetActiveWindow(), &rect);
      FillRect(hdcBuffer, &rect, hBrush);
      DeleteObject(hBrush);

      // Render all sprites
      for (auto& sprite : sprites) {
        sprite->Render(hdcBuffer);
      }
      
      // Copy buffer to screen
      HDC hdc = GetDC(GetActiveWindow());
      BitBlt(hdc, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, hdcBuffer, 0, 0, SRCCOPY);
      ReleaseDC(GetActiveWindow(), hdc);

      // Force a redraw
      if (needsRedraw) {
        InvalidateRect(GetActiveWindow(), NULL, FALSE);
      }

      Sleep(1);
    }
  }

    return (int)msg.wParam;
}