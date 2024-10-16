#include <windows.h>

void onPaint(HWND hWnd) {
  PAINTSTRUCT ps;
  HDC hdc = BeginPaint(hWnd, &ps);

  RECT rect;
  GetClientRect(hWnd, &rect);

  int windowWidth = rect.right - rect.left;
  int windowHeight = rect.bottom - rect.top;

  HDC memDC = CreateCompatibleDC(hdc);
  HBITMAP memBitmap = CreateCompatibleBitmap(hdc, windowWidth, windowHeight);
  SelectObject(memDC, memBitmap);

  FillRect(memDC, &rect, (HBRUSH)(COLOR_WINDOW + 1));
  Ellipse(memDC, 100, 100, 300, 300);

  BitBlt(hdc, 0, 0, windowWidth, windowHeight, memDC, 0, 0, SRCCOPY);

  DeleteObject(memBitmap);
  DeleteDC(memDC);
  EndPaint(hWnd, &ps);
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
  switch (message) {
    case WM_DESTROY:
      PostQuitMessage(0);
      return 0;
    case WM_PAINT: {
      onPaint(hWnd);
      return 0;
    }
    case WM_TIMER: {
      if (wParam == 1) {
        MessageBox(hWnd, "Timer triggered!", "Timer", MB_OK);
        KillTimer(hWnd, 1);
      }
      return 0;
    }
    default:
      return DefWindowProc(hWnd, message, wParam, lParam);
  }
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
  WNDCLASS wc = {};
  wc.lpfnWndProc = WndProc;
  wc.lpszClassName = "MyWindowClass";
  wc.hInstance = hInstance;
  RegisterClass(&wc);


  HWND hWnd = CreateWindowEx(0, wc.lpszClassName, "Title?", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 800, 600, NULL, NULL, hInstance, NULL);
  ShowWindow(hWnd, nCmdShow);
  SetTimer(hWnd, 1, 2000, NULL);

  MSG msg;
  while (GetMessage(&msg, NULL, 0, 0)) {
    TranslateMessage(&msg);
    DispatchMessage(&msg);
  }

  return 0;
}