#include <windows.h>

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
  HDC hdc;
  switch (message) {
    case WM_DESTROY:
      PostQuitMessage(0);
      return 0;
    case WM_ERASEBKGND: {
      hdc = (HDC)wParam;
      RECT clientRect;
      GetClientRect(hWnd, &clientRect);
      HBRUSH hBrush = CreateSolidBrush(RGB(255, 0, 0));
      FillRect(hdc, &clientRect, hBrush);
      return 1;
    }
    case WM_PAINT: {
      PAINTSTRUCT ps;
      hdc = BeginPaint(hWnd, &ps);
      RECT rect;
      GetClientRect(hWnd, &rect);
      SetBkMode(hdc, TRANSPARENT);
      HFONT hFont = CreateFont(
        48, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
        OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
        DEFAULT_PITCH | FF_SWISS, TEXT("liSu")
      );
      SelectObject(hdc, hFont);
      SetTextColor(hdc, RGB(255, 255, 0));
      DrawTextW(hdc, L"国庆快乐!", -1, &rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

      DeleteObject(hFont);
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
    TEXT("Hello, World!"),
    WS_OVERLAPPEDWINDOW,
    CW_USEDEFAULT, CW_USEDEFAULT,
    800, 600,
    NULL,
    NULL,
    hInstance,
    NULL
  );

  ShowWindow(hWnd, nCmdShow);
  
  MSG msg;
  while (GetMessage(&msg, NULL, 0, 0)) {
    TranslateMessage(&msg);
    DispatchMessage(&msg);
  }
  return 0;
}