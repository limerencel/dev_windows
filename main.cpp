#include <windows.h>

//Window Procedure
LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
  switch (message) {
    case WM_DESTROY:
      PostQuitMessage(0);
      return 0;
  
    case WM_PAINT: {
      PAINTSTRUCT ps;
      HDC hdc = BeginPaint(hwnd, &ps); //get device conetxt for painting
      //File the window wiht a solid color
      FillRect(hdc, &ps.rcPaint, (HBRUSH)(COLOR_WINDOW + 1));

      EndPaint(hwnd, &ps);
      return 0;
    }
    default:
      return DefWindowProc(hwnd, message, wParam, lParam);
  }

  return DefWindowProc(hwnd, message, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int cmdShow) {
  //register the window class
  WNDCLASS wc = {};
  wc.lpfnWndProc = WndProc;
  wc.hInstance = hInstance;
  wc.lpszClassName = "MyWindowClass";

  RegisterClass(&wc);

  //create the window
  HWND hwnd = CreateWindowEx(
    0,
    wc.lpszClassName,
    "Hello, World!",
    WS_OVERLAPPEDWINDOW,
    CW_USEDEFAULT,
    CW_USEDEFAULT,
    800,
    600,
    NULL,
    NULL,
    wc.hInstance,
    NULL
  );
  if (hwnd == NULL) {
    return 0; //hndle error
  }

  ShowWindow(hwnd, cmdShow);

  // Message loop
  MSG msg;
  while (GetMessage(&msg, NULL, 0, 0)) {
    TranslateMessage(&msg);
    DispatchMessage(&msg);
  }

  return 0;
}