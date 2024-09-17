#define unicode
#define _unicode
#include <windows.h>

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
  switch (message) {
    case WM_CLOSE:
      PostQuitMessage(0);
      break;
    case WM_PAINT: {
      PAINTSTRUCT ps;
      HDC hdc = BeginPaint(hWnd, &ps);
      HBRUSH starBrush = CreateSolidBrush(RGB(255, 255, 0));
      
      // draw a star pattern
      POINT starPoints[10] = {
          {150, 25}, {179, 111}, {269, 111}, {197, 165},
          {223, 251}, {150, 200}, {77, 251}, {103, 165},
          {31, 111}, {121, 111}
      };
      float scale = 0.5f;
      for (int i = 0; i < 10; i++) {
        starPoints[i].x = static_cast<LONG>(starPoints[i].x * scale);
        starPoints[i].y = static_cast<LONG>(starPoints[i].y * scale);
      }
      SelectObject(hdc, starBrush);
      Polygon(hdc, starPoints, 10);
      DeleteObject(starBrush);

      EndPaint(hWnd, &ps);
      break;
    }
    default:
      return DefWindowProc(hWnd, message, wParam, lParam);
  }
  return 0;

}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
  // Register class
  WNDCLASS wc = {};
  wc.lpfnWndProc = WndProc;
  wc.hInstance = hInstance;
  wc.lpszClassName = "MyClass";
  RegisterClass(&wc);
  
  // Create window
  HWND hWnd = CreateWindowEx(
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
    hInstance,
    NULL);

  if (hWnd == NULL) {
    MessageBox(NULL, "Failed to create window", "Error", MB_ICONERROR);
    return 0;
  }


  ShowWindow(hWnd, nCmdShow);

  // Message loop
  MSG msg;
  while (GetMessage(&msg, NULL, 0, 0)) {
    TranslateMessage(&msg);
    DispatchMessage(&msg);
  }

  return 0;
}