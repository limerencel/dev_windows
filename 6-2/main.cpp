#include <windows.h>
#include "Monster.h"
#include <wingdi.h>

const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 600;

Monster g_monster; // g: global variable; Automatic (Stack) Allocation
void Render(HWND hWnd);

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
  switch(message) {
    case WM_CREATE:
      {
        g_monster.Initialize(GetModuleHandle(NULL), "./monster.bmp");
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
  PAINTSTRUCT ps;
  HDC hdc = BeginPaint(hWnd, &ps);
  
  // Create a memory DC and bitmap for double-buffering
  HDC memDC = CreateCompatibleDC(hdc);
  HBITMAP hBitmap = CreateCompatibleBitmap(hdc, WINDOW_WIDTH, WINDOW_HEIGHT);
  HBITMAP hOldBitmap = (HBITMAP)SelectObject(memDC, hBitmap);

  // Clear the background
  HBRUSH hBrush = CreateSolidBrush(RGB(255, 255, 255));
  RECT rc = {0, 0, WINDOW_WIDTH, WINDOW_HEIGHT};
  FillRect(memDC, &rc, hBrush);
  DeleteObject(hBrush);

  // Render the sprite to the memory DC
  COLORREF transparentColor = RGB(0, 229, 0);

  // Get the dimensions of the sprite
  int spriteWidth = g_monster.GetWidth();
  int spriteHeight = g_monster.GetHeight();

  // Copy the memory DC to the screen DC
  TransparentBlt(
    memDC,
    g_monster.GetX(),
    g_monster.GetY(),
    spriteWidth,
    spriteHeight,
    g_monster.GetMemDC(),
    0, 0,
    spriteWidth,
    spriteHeight,
    transparentColor
  );

  BitBlt(hdc, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, memDC, 0, 0, SRCCOPY);

  // Clean up
  SelectObject(memDC, hOldBitmap);
  DeleteObject(hBitmap);
  DeleteDC(memDC);

  EndPaint(hWnd, &ps);
}

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
  WNDCLASS wc = {};
  wc.lpfnWndProc = WndProc;
  wc.hInstance = hInstance;
  wc.lpszClassName = "MyWindowClass";

  RegisterClass(&wc);

  HWND hWnd = CreateWindowEx(
    0,
    wc.lpszClassName,
    "Hello, Windows!",
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
  while (true) {
    if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
      if (msg.message == WM_QUIT) break;
      TranslateMessage(&msg);
      DispatchMessage(&msg);
    } else {
      // Get Client area dimensions
      GetClientRect(hWnd, &clientRect);
      int width = clientRect.right - clientRect.left;
      int height = clientRect.bottom - clientRect.top;

      // Game logic
      g_monster.Update(width, height);

      // Render
      InvalidateRect(hWnd, NULL, FALSE);
      Render(hWnd);
    }
  }

  return (int)msg.wParam;
}