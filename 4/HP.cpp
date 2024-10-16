#include <windows.h>

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
  static int width = 200;
  static int height = 20;
  static int x = 100;
  static int y = 100;
  static COLORREF greenColor = RGB(0, 255, 0);
  static COLORREF redColor = RGB(255, 0, 0);
  static RECT rect;

  switch (message) {
    case WM_DESTROY:
      PostQuitMessage(0);
      return 0;
    case WM_PAINT: {
      PAINTSTRUCT ps;
      HDC hdc = BeginPaint(hWnd, &ps);

      RECT backgroundRect = {100, 100, 300, 120};
      HBRUSH hBackgroundBrush = CreateSolidBrush(greenColor);
      HPEN hPen = CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
      HBRUSH hOldBrush = (HBRUSH)SelectObject(hdc, hBackgroundBrush);
      HPEN hOldPen = (HPEN)SelectObject(hdc, hPen);
      Rectangle(hdc, backgroundRect.left, backgroundRect.top, backgroundRect.right, backgroundRect.bottom);
      DeleteObject(hBackgroundBrush);

      SelectObject(hdc, hOldPen);
      SelectObject(hdc, hOldBrush);
      DeleteObject(hBackgroundBrush);
      DeleteObject(hPen);


      rect.left = x;
      rect.top = y;
      rect.right = x + width;
      rect.bottom = y + height;

      HBRUSH hBrush = CreateSolidBrush(redColor);
      hPen = CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
      hOldBrush = (HBRUSH)SelectObject(hdc, hBrush);
      hOldPen = (HPEN)SelectObject(hdc, hPen);
      Rectangle(hdc, rect.left, rect.top, rect.right, rect.bottom);

      SelectObject(hdc, hOldPen);
      SelectObject(hdc, hOldBrush);
      DeleteObject(hBrush);
      DeleteObject(hPen);

      EndPaint(hWnd, &ps);
      return 0;
    }
    case WM_TIMER:
      if (width > 0) {
        width--;
        InvalidateRect(hWnd, NULL, FALSE);
      }
      return 0;
    default:
      return DefWindowProc(hWnd, message, wParam, lParam);
  }
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lmCmdLine, int nCmdShow) {
  WNDCLASS wc = {};
  wc.hInstance = hInstance;
  wc.lpfnWndProc = WndProc;
  wc.lpszClassName = "showHP";

  RegisterClass(&wc);

  HWND hWnd = CreateWindowEx(
    0,
    wc.lpszClassName,
    "Show HP",
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
  SetTimer(hWnd, 1, 50, NULL);

  MSG msg;
  while (GetMessage(&msg, NULL, 0, 0)) {
    TranslateMessage(&msg);
    DispatchMessage(&msg);
  }

  return 0;
}