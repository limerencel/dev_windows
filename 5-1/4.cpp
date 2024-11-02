#include <windows.h>

HBRUSH hBrush;
RECT rect1 = { 100, 100, 300, 300 };
RECT rect2 = { 400, 200, 600, 400 };
BOOL isFilled1 = FALSE;
BOOL isFilled2 = FALSE;
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
  switch (message) {
    case WM_CREATE:
      hBrush = CreateSolidBrush(RGB(0, 0, 0));
      break;
    case WM_PAINT: {
      PAINTSTRUCT ps;
      HDC hdc = BeginPaint(hWnd, &ps);

      if (isFilled1) {
        FillRect(hdc, &rect1, hBrush);
      } else {
        Rectangle(hdc, rect1.left, rect1.top, rect1.right, rect1.bottom);
      }

      if (isFilled2) {
        FillRect(hdc, &rect2, hBrush);
      } else {
        Rectangle(hdc, rect2.left, rect2.top, rect2.right, rect2.bottom);
      }

      EndPaint(hWnd, &ps);
      break;
    }
    case WM_LBUTTONDOWN: {
      int xPos = LOWORD(lParam);
      int yPos = HIWORD(lParam);

      if (xPos >= rect1.left && xPos <= rect1.right && yPos >= rect1.top && yPos <= rect1.bottom) {
        isFilled1 = TRUE;
      }

      if (xPos >= rect2.left && xPos <= rect2.right && yPos >= rect2.top && yPos <= rect2.bottom) {
        isFilled2 = TRUE;
      }
      InvalidateRect(hWnd, NULL, TRUE);
      break;
    }
    case WM_DESTROY:
      DeleteObject(hBrush);
      PostQuitMessage(0);
    default:
      return DefWindowProc(hWnd, message, wParam, lParam);
  }
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    WNDCLASS wc = {};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = "rect";

    RegisterClass(&wc);

    HWND hWnd = CreateWindowEx(
    0,
    wc.lpszClassName,
    "Rect",
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
