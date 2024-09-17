#define UNICODE
#define _UNICODE
#include <windows.h>

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
  switch (message) {
    case WM_CLOSE:
      PostQuitMessage(0);
      return 0;
    case WM_ERASEBKGND: {
      HDC hdc = (HDC)wParam;
      RECT clientRect;
      GetClientRect(hWnd, &clientRect);
      FillRect(hdc, &clientRect, (HBRUSH)(COLOR_WINDOW + 1));
      return 0;
    }
    case WM_PAINT: {
      PAINTSTRUCT ps;
      HDC hdc = BeginPaint(hWnd, &ps);

      // create the scoreboard
      HBRUSH scoreboardBrush = CreateSolidBrush(RGB(110, 194, 7));
      HBRUSH chessboardBrush = CreateSolidBrush(RGB(255, 136, 91));
      HBRUSH scoreBoxBrush = CreateSolidBrush(RGB(145, 79, 30));
      HPEN scoreboardBorderPen = CreatePen(PS_SOLID, 2, RGB(123, 225, 7));
      HPEN chessboardBorderPen = CreatePen(PS_SOLID, 2, RGB(255, 174, 144));
      HPEN scoreBoxPen = CreatePen(PS_SOLID, 2, RGB(222, 172, 128));

      // Get the client area dimensions
      RECT clientRect;
      GetClientRect(hWnd, &clientRect);

      // fixed width, padding and radius
      int fixedWidth = clientRect.right / 4;
      int padding = 16;
      int borderRadius = 20;
      int scoreBoxRadius = 10;

      // draw the scoreboard
      RECT scoreboardRect = { padding, padding+7, fixedWidth-padding, clientRect.bottom-padding-7 };
      SelectObject(hdc, scoreboardBrush);
      SelectObject(hdc, scoreboardBorderPen);
      RoundRect(hdc, scoreboardRect.left, scoreboardRect.top, scoreboardRect.right, scoreboardRect.bottom, borderRadius, borderRadius);
      // FillRect(hdc, &scoreboardRect, scoreboardBrush);

      // draw the chessboard
      RECT chessboardRect = { fixedWidth, padding+7, clientRect.right-padding, clientRect.bottom-padding-7 };
      SelectObject(hdc, chessboardBrush);
      SelectObject(hdc, chessboardBorderPen);
      RoundRect(hdc, chessboardRect.left, chessboardRect.top, chessboardRect.right, chessboardRect.bottom, borderRadius, borderRadius);
      // FillRect(hdc, &chessboardRect, chessboardBrush);

      // Draw boxes of playes' scores
      RECT topScoreRect = {fixedWidth / 6, 400, fixedWidth / 6 * 5, 430};
      SelectObject(hdc, scoreBoxBrush);
      SelectObject(hdc, scoreBoxPen);
      RoundRect(hdc, topScoreRect.left, topScoreRect.top, topScoreRect.right, topScoreRect.bottom, scoreBoxRadius, scoreBoxRadius);
      RECT bottomScoreRect = {fixedWidth / 6, 450, fixedWidth / 6 * 5, 480};
      RoundRect(hdc, bottomScoreRect.left, bottomScoreRect.top, bottomScoreRect.right, bottomScoreRect.bottom, scoreBoxRadius, scoreBoxRadius);

      // Draw chessboard lines
      SelectObject(hdc, CreatePen(PS_SOLID, 1, RGB(0, 0, 0)));
      int squareSize = (chessboardRect.right - chessboardRect.left) / 10;
      for (int i = 1; i < 10; i++) {
        // horizontal lines
        if (i < 9) {
          MoveToEx(hdc, chessboardRect.left, chessboardRect.top + i * squareSize, NULL);
          LineTo(hdc, chessboardRect.right, chessboardRect.top + i * squareSize);
        }

        // vertical lines
        MoveToEx(hdc, chessboardRect.left + i * squareSize, chessboardRect.top, NULL);
        LineTo(hdc, chessboardRect.left + i * squareSize, chessboardRect.bottom);
      }

      // Clean up
      DeleteObject(scoreboardBorderPen);
      DeleteObject(chessboardBorderPen);
      DeleteObject(scoreBoxPen);
      DeleteObject(chessboardBrush);
      DeleteObject(scoreboardBrush);
      DeleteObject(scoreBoxBrush);
      EndPaint(hWnd, &ps);
      return 0;
    }
    default: 
      return DefWindowProc(hWnd, message, wParam, lParam);
  }
}

int WINAPI WinMain(HINSTANCE instance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
  // Register the window class
  const wchar_t CLASS_NAME[] = L"Scoreboard";

  WNDCLASS wc = { };
  wc.lpfnWndProc = WndProc;
  wc.hInstance = instance;
  wc.lpszClassName = CLASS_NAME;

  RegisterClass(&wc);

  // Create the window
  HWND hwnd = CreateWindowEx(
    0,
    CLASS_NAME,
    L"Scoreboard",
    WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
    CW_USEDEFAULT, CW_USEDEFAULT,
    800,
    600,
    NULL,
    NULL,
    instance,
    NULL
  );

  if (hwnd == NULL) {
    MessageBox(NULL, L"Window creation failed!", L"Error", MB_ICONERROR);
    return 0;
  }

  ShowWindow(hwnd, nCmdShow);

  // run the message loop
  MSG msg = {};
  while (GetMessage(&msg, NULL, 0, 0) > 0) {
    TranslateMessage(&msg);
    DispatchMessage(&msg);
  }

  return 0;
}