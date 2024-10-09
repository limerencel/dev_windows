#include <windows.h>

HPEN hPen_Blue, hPen_Black, hPen_Red, hPen_Yellow, hPen_Green;
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
  HDC hdc;
  switch(message) {
    case WM_CREATE: {
      hPen_Blue = CreatePen(PS_SOLID, 5, RGB(0, 0, 255));
      hPen_Black = CreatePen(PS_SOLID, 5, RGB(0, 0, 0));
      hPen_Red = CreatePen(PS_SOLID, 5, RGB(255, 0, 0));
      hPen_Yellow = CreatePen(PS_SOLID, 5, RGB(255, 255, 0));
      hPen_Green = CreatePen(PS_SOLID, 5, RGB(0, 255, 0));
      return 0;
    }
    case WM_DESTROY:
      DeleteObject(hPen_Blue);
      DeleteObject(hPen_Black);
      DeleteObject(hPen_Red);
      DeleteObject(hPen_Yellow);
      DeleteObject(hPen_Green);
      PostQuitMessage(0);
      return 0;
    case WM_ERASEBKGND:
      hdc = (HDC)wParam;
      RECT rc;
      GetClientRect(hWnd, &rc);
      FillRect(hdc, &rc, (HBRUSH)(COLOR_WINDOW + 1));
      return 1;
    case WM_PAINT: {
      PAINTSTRUCT ps;
      hdc = BeginPaint(hWnd, &ps);

      SetBkMode(hdc, TRANSPARENT);
      HBRUSH hBrushNull = (HBRUSH)GetStockObject(NULL_BRUSH);

      SelectObject(hdc, hBrushNull);
      HPEN Blue = (HPEN)SelectObject(hdc, hPen_Blue);
      Ellipse(hdc, 100, 100, 200, 200);
      SelectObject(hdc, Blue);
      HPEN Black = (HPEN)SelectObject(hdc, hPen_Black);
      Ellipse(hdc, 180, 100, 280, 200);
      SelectObject(hdc, Black);
      HPEN Red = (HPEN)SelectObject(hdc, hPen_Red);
      Ellipse(hdc, 260, 100, 360, 200);
      SelectObject(hdc, Red);
      HPEN Yellow = (HPEN)SelectObject(hdc, hPen_Yellow);
      Ellipse(hdc, 140, 180, 240, 280);
      SelectObject(hdc, Yellow);
      HPEN Green = (HPEN)SelectObject(hdc, hPen_Green);
      Ellipse(hdc, 220, 180, 320, 280);
      SelectObject(hdc, Green);

      SelectObject(hdc, GetStockObject(BLACK_BRUSH));

      EndPaint(hWnd, &ps);
      return 0;
    }
    default:
      return DefWindowProc(hWnd, message, wParam, lParam);
  }
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
  WNDCLASS wc = {};
  wc.lpfnWndProc = WndProc;
  wc.hInstance = hInstance;
  wc.lpszClassName = TEXT("MyWindowClass");

  RegisterClass(&wc);

  HWND hWnd = CreateWindowEx(
    0,
    wc.lpszClassName,
    TEXT("Circle Drawing"),
    WS_OVERLAPPEDWINDOW,
    CW_USEDEFAULT, CW_USEDEFAULT,
    800, 600, NULL, NULL, hInstance, NULL
  );

  ShowWindow(hWnd, nCmdShow);

  MSG msg;
  while (GetMessage(&msg, NULL, 0, 0)) {
    TranslateMessage(&msg);
    DispatchMessage(&msg);
  }

  return 0;
}