#include <windows.h>
#include "Monster.h"
#include <wingdi.h>
#include <iostream>
#include <sstream>
#include "Resource.h"

const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 600;

Monster g_monster; // g: global variable; Automatic (Stack) Allocation
void Render(HWND hWnd);

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
  switch(message) {
    case WM_CREATE:
      {
        g_monster.Initialize(GetModuleHandle(NULL), IDB_BITMAP1);
      }
      break;
    case WM_PAINT: {
      Render(hWnd);
      break;
    }
    case WM_DESTROY:
      PostQuitMessage(0);
      break;
    default:
      return DefWindowProc(hWnd, message, wParam, lParam);
  }
}

void Render(HWND hWnd) {
  RECT clientRect;
  GetClientRect(hWnd, &clientRect);
  int width = clientRect.right - clientRect.left;
  int height = clientRect.bottom - clientRect.top;

  HDC hdc = GetDC(hWnd);
  
  // Create a memory DC and bitmap for double-buffering
  HDC memDC = CreateCompatibleDC(hdc);
  HBITMAP hBitmap = CreateCompatibleBitmap(hdc, width, height);
  HBITMAP hOldBitmap = (HBITMAP)SelectObject(memDC, hBitmap);

  // Clear the background
  HBRUSH hBrush = CreateSolidBrush(RGB(255, 255, 255));
  FillRect(memDC, &clientRect, hBrush);
  DeleteObject(hBrush);

  // Render the monster
  g_monster.Render(memDC);

  // Copy the memory DC to the screen DC
  BitBlt(hdc, 0, 0, width, height, memDC, 0, 0, SRCCOPY);

  // Clean up
  SelectObject(memDC, hOldBitmap);
  DeleteObject(hBitmap);
  DeleteDC(memDC);

  ReleaseDC(hWnd, hdc);
}

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
  WNDCLASS wc = {};
  wc.lpfnWndProc = WndProc;
  wc.hInstance = hInstance;
  wc.lpszClassName = L"MyWindowClass";

  RegisterClass(&wc);

  HWND hWnd = CreateWindowEx(
    0,
    wc.lpszClassName,
    L"Hello, Windows!",
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

  ShowWindow(hWnd, nCmdShow);
  UpdateWindow(hWnd);

  MSG msg;
  RECT clientRect;
  DWORD lastUpdateTime = GetTickCount64();
  const DWORD updateInterval = 1000 / 60; // 60 FPS

  while (true) {
    if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
      if (msg.message == WM_QUIT) break;
      TranslateMessage(&msg);
      DispatchMessage(&msg);
    } else {
      DWORD currentTime = GetTickCount64();
      if (currentTime - lastUpdateTime >= updateInterval) {
        // Get Client area dimensions
        GetClientRect(hWnd, &clientRect);
        int width = clientRect.right - clientRect.left;
        int height = clientRect.bottom - clientRect.top;

        // Game logic
        g_monster.Update(width, height);

        std::cout << "Monster position: (" << g_monster.GetX() << ", " << g_monster.GetY() << ")" << std::endl;

        // Render
        Render(hWnd);

        lastUpdateTime = currentTime;
      } else {
        Sleep(1);
      }
    }
  }

  return (int)msg.wParam;
}