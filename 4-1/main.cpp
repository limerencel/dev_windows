#include <windows.h>

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
  static HBITMAP hBitmap;

  switch (message) {
    case WM_CREATE: {
      hBitmap = (HBITMAP)LoadImage(
        NULL,
        TEXT("./bitmap/walker.bmp"),
        IMAGE_BITMAP,
        0,
        0,
        LR_LOADFROMFILE | LR_DEFAULTSIZE
      );
      if (hBitmap == NULL) MessageBox(hWnd, TEXT("Failed to load bitmap."), TEXT("Error"), MB_OK | MB_ICONERROR);
      break;
    }
    case WM_PAINT: {
      PAINTSTRUCT ps;
      HDC hdc = BeginPaint(hWnd, &ps);
      // draw bitmap
      if (hBitmap) {
        HDC hdcMem = CreateCompatibleDC(hdc);
        HBITMAP hOldBitMap = (HBITMAP)SelectObject(hdcMem, hBitmap);
        BITMAP bitmap;
        GetObject(hBitmap, sizeof(BITMAP), &bitmap);
        // draw bitmap to the window
        BitBlt(hdc, 0, 0, bitmap.bmWidth, bitmap.bmHeight, hdcMem, 0, 0, SRCCOPY);
        SelectObject(hdcMem, hOldBitMap);
        DeleteDC(hdcMem);
      }
      EndPaint(hWnd, &ps);
      return 0;
    }
    case WM_DESTROY:
      if (hBitmap) {
        DeleteObject(hBitmap);
      }
      PostQuitMessage(0);
      return 0;
  }
  return DefWindowProc(hWnd, message, wParam, lParam);
}

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
  // register window class
  WNDCLASSEX wc = {};
  wc.cbSize = sizeof(WNDCLASSEX);
  wc.lpfnWndProc = WndProc;
  wc.hInstance = hInstance;
  wc.lpszClassName = TEXT("BitMapWindow");
  wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
  wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
  wc.hCursor = LoadCursor(NULL, IDC_ARROW);
  wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
  if (!RegisterClassEx(&wc)) {
    DWORD errorCode = GetLastError();
    MessageBox(NULL, TEXT("Failed to register window class!"), TEXT("Error"), MB_OK | MB_ICONERROR);
    return -1;
  }

  //create window
  HWND hWnd = CreateWindow(
    TEXT("BitMapWindow"),
    TEXT("Display Bitmap from file"),
    WS_OVERLAPPEDWINDOW,
    CW_USEDEFAULT, CW_USEDEFAULT,
    800, 600,
    NULL,
    NULL,
    hInstance,
    NULL
  );

  if (hWnd == NULL) {
    DWORD errorCode = GetLastError();
    MessageBox(NULL, TEXT("Failed to create window!"), TEXT("Error"), MB_OK | MB_ICONERROR);
    return -1;
  }
    ShowWindow(hWnd, nCmdShow);

  // message loop
  MSG msg;
  while (GetMessage(&msg, NULL, 0, 0)) {
    TranslateMessage(&msg);
    DispatchMessage(&msg);
  }

  return 0;
}