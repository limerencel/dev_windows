#include <windows.h>

HBITMAP hBitMap;
BITMAP bitmap;
int x = 100, y = 100;
COLORREF transparentColor = RGB(0, 229, 0);

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
  switch (message) {
    case WM_CREATE:
      hBitMap = (HBITMAP)LoadImage(NULL, "./monster.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
      break;
    case WM_PAINT: {
      PAINTSTRUCT ps;
      HDC hdc = BeginPaint(hWnd, &ps);
      HDC memDC = CreateCompatibleDC(hdc);
      HBITMAP oldBitmap = (HBITMAP)SelectObject(memDC, hBitMap);
      FillRect(hdc, &ps.rcPaint, (HBRUSH)(COLOR_WINDOW + 1));
      GetObject(hBitMap, sizeof(bitmap), &bitmap);
      TransparentBlt(hdc, x, y, bitmap.bmWidth, bitmap.bmHeight, memDC, 0, 0, bitmap.bmWidth, bitmap.bmHeight, transparentColor);
      SelectObject(memDC, oldBitmap);
      DeleteDC(memDC);
      EndPaint(hWnd, &ps);
      break;
    }
    case WM_KEYDOWN:
      switch(wParam) {
        case VK_LEFT: x -= 10; break;
        case VK_RIGHT: x += 10; break;
        case VK_UP: y -= 10; break;
        case VK_DOWN: y += 10; break;
        case VK_HOME: x = 0; y = 0; break;
        case VK_END: {
          RECT clientRect;
          GetClientRect(hWnd, &clientRect);
          x = clientRect.right - bitmap.bmWidth;
          y = clientRect.bottom - bitmap.bmHeight;
          break;
        }
      }
      InvalidateRect(hWnd, NULL, TRUE);
      break;
    case WM_DESTROY:
      DeleteObject(hBitMap);
      PostQuitMessage(0);
      break;
    default:
      return DefWindowProc(hWnd, message, wParam, lParam);
  }
  return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
  WNDCLASS wc = {};
  wc.lpfnWndProc = WndProc;
  wc.hInstance = hInstance;
  wc.lpszClassName = "MyWindowClass";

  RegisterClass(&wc);

  HWND hWnd = CreateWindowEx(
    0,
    wc.lpszClassName,
    "Control Monster",
    WS_OVERLAPPEDWINDOW,
    CW_USEDEFAULT,
    CW_USEDEFAULT,
    800,
    600,
    NULL,
    NULL,
    hInstance,
    NULL
  );

  ShowWindow(hWnd, nCmdShow);
  UpdateWindow(hWnd);

  MSG msg;
  while (GetMessage(&msg, NULL, 0, 0)) {
    TranslateMessage(&msg);
    DispatchMessage(&msg);
  }

  return 0;
}