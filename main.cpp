#define UNICODE
#define _UNICODE
#include <windows.h>
#include <string>
#include <cstdio>

//Window Procedure
int counter = 0;
LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
  switch (message) {
    case WM_CLOSE:
      {
      wchar_t  message[100];
      wsprintf(message, L"GetMessage: %d times.", counter);
      MessageBox(hwnd, message, L"Message", MB_OK);
      }
      PostQuitMessage(0);
      return 0;
  
    case WM_PAINT: {
      PAINTSTRUCT ps;
      HDC hdc = BeginPaint(hwnd, &ps); //get device conetxt for painting
      //File the window wiht a solid color
      HBRUSH hBrush = CreateSolidBrush(RGB(0,0,0));
      // FillRect(hdc, &ps.rcPaint, (HBRUSH)(COLOR_WINDOW + 1));
      FillRect(hdc, &ps.rcPaint, hBrush);
      DeleteObject(hBrush);

      SetTextColor(hdc, RGB(255, 255, 255));
      SetBkMode(hdc, TRANSPARENT);
      TextOutW(hdc, 20, 40, L"Hello, Windows!", wcslen(L"Hello, Windows!"));

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
  wc.lpszClassName = L"MyWindowClass";
  wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
  wc.hCursor = LoadCursor(NULL, IDC_HAND);

  RegisterClass(&wc);

  // create the window
  HWND hwnd = CreateWindowEx(
    0,
    wc.lpszClassName,
    L"Numbers...",
    WS_OVERLAPPEDWINDOW,
    100,
    200,
    400,
    300,
    NULL,
    NULL,
    wc.hInstance,
    NULL
  );
  if (hwnd == NULL) {
    return 0; //hndle error
  }


  ShowWindow(hwnd, cmdShow);

  // Message loop for GetMessage
  // MSG msg = {};
  // int result;
  // while ((result = GetMessage(&msg, NULL, 0, 0)) != 0) {
  //   if (result == -1) {
  //     break; // handle the error
  //   }
  //   counter++;
  //   TranslateMessage(&msg);
  //   DispatchMessage(&msg);
  // }
  
  // for PeekMessage
  MSG msg = {};
  while (true) {
    if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)!= 0) {
      if (msg.message == WM_QUIT) {
        break;
      }
      TranslateMessage(&msg);
      DispatchMessage(&msg);
    } else {
      counter++;
      Sleep(1000); // prevent heavy CPU usage
    }
  }

  return 0;
}