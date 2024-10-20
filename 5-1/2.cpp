#include <windows.h>

int x = 100;
int y = 100;
HBITMAP hBitMap;

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
  switch (message) {
    case WM_CREATE:
      hBitMap = (HBITMAP)LoadImage(NULL, "./car.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
      SetTimer(hWnd, 1, 16, NULL);
      break;
    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);
        HDC memDC = CreateCompatibleDC(hdc);

        HBITMAP offscreenBitmap = CreateCompatibleBitmap(hdc, ps.rcPaint.right - ps.rcPaint.left, ps.rcPaint.bottom - ps.rcPaint.top);
        HBITMAP oldBitmap = (HBITMAP)SelectObject(memDC, offscreenBitmap);

        FillRect(memDC, &ps.rcPaint, (HBRUSH)(COLOR_WINDOW + 1));

        BITMAP bitmap;
        GetObject(hBitMap, sizeof(BITMAP), &bitmap);

        HDC bitmapDC = CreateCompatibleDC(hdc);
        HBITMAP oldBitmapDC = (HBITMAP)SelectObject(bitmapDC, hBitMap);
        BitBlt(memDC, x, y, bitmap.bmWidth, bitmap.bmHeight, bitmapDC, 0, 0, SRCCOPY);

        BitBlt(hdc, ps.rcPaint.left, ps.rcPaint.top, ps.rcPaint.right - ps.rcPaint.left, ps.rcPaint.bottom - ps.rcPaint.top, memDC, 0, 0, SRCCOPY);

        SelectObject(bitmapDC, oldBitmapDC);
        DeleteDC(bitmapDC);
        SelectObject(memDC, oldBitmap); 
        DeleteObject(offscreenBitmap);
        DeleteDC(memDC);

        
        EndPaint(hWnd, &ps);
        break;
      }

    case WM_TIMER:
      if (GetKeyState(VK_LEFT) & 0x8000) x -= 4; InvalidateRect(hWnd, NULL, TRUE); break;
    case WM_KEYDOWN:
      switch(wParam) {
        case VK_RIGHT: x += 5; break;
      }
      InvalidateRect(hWnd, NULL, TRUE);
      break;
    case WM_DESTROY:
      DeleteObject(hBitMap);
      KillTimer(hWnd, 1);
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
  // wc.style = CS_HREDRAW | CS_VREDRAW | WS_EX_COMPOSITED;
  

  RegisterClass(&wc);

  HWND hWnd = CreateWindowEx(
    0,
    wc.lpszClassName,
    "My First Window",
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