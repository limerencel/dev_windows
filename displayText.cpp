#include <windows.h>

HFONT hFont1, hFont2, quoteFont, chineseFonts[3], chineseFont;
int currentChineseFontIndex = 0;
void drawParagraph(HDC hdc, const wchar_t* paragraph, RECT* rect, COLORREF bgColor, COLORREF textColor, HFONT hFont) {
  // set background color and fill the rectangle
  HBRUSH hBrush = CreateSolidBrush(bgColor);
  FillRect(hdc, rect, hBrush);
  DeleteObject(hBrush);

  // set text color and font
  SetTextColor(hdc, textColor);
  SelectObject(hdc, hFont);
  SetBkMode(hdc, TRANSPARENT);

  // draw the text inside the rectangle
  DrawTextW(hdc, paragraph, -1, rect, DT_LEFT | DT_WORDBREAK);
}


void displatFormattedText(HDC hdc, const char* text, int x, int y, int width, int height) {
  RECT rect = {x, y, x + width, y + height};
  SetBkMode(hdc, TRANSPARENT);
  SetTextColor(hdc, RGB(255, 0, 0));
  DrawText(hdc, text, -1, &rect, DT_CENTER | DT_WORDBREAK);
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
  switch (message) {
    case WM_CREATE: {
      hFont1 = CreateFont(30, 0, 0, 0, FW_BOLD, FALSE, FALSE, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, "Arial");
      hFont2 = CreateFont(-20, 0, 0, 0, FW_NORMAL, TRUE, FALSE, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, "Times New Roman");
      quoteFont = CreateFont(-22, 0, 0, 0, FW_NORMAL, TRUE, FALSE, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, "Times New Roman");
      chineseFonts[0] = CreateFont(30, 0, 0, 0, FW_NORMAL, FALSE, FALSE, 0, GB2312_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, "STFangSong");
      chineseFonts[1] = CreateFont(30, 0, 0, 0, FW_NORMAL, FALSE, FALSE, 0, GB2312_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, "SimHei");
      chineseFonts[2] = CreateFont(30, 15, 0, 0, FW_NORMAL, TRUE, FALSE, 0, GB2312_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, "SimSun");
      chineseFont = chineseFonts[0];
      return 0;
    }
    case WM_CLOSE:
      if (MessageBox(hwnd, "Are you sure you want to exit?", "Exit", MB_YESNO) == IDYES) {
        DestroyWindow(hwnd);
      }
      return 0;
    case WM_DESTROY:
      DeleteObject(hFont1);
      DeleteObject(hFont2);
      PostQuitMessage(0);
      return 0;
    case WM_ERASEBKGND: {
      HDC hdc = (HDC)wParam;
      RECT clientRect;
      GetClientRect(hwnd, &clientRect);
      FillRect(hdc, &clientRect, (HBRUSH)(COLOR_WINDOW + 1));
      return 0;
    }
    case WM_LBUTTONDOWN:
      currentChineseFontIndex = (currentChineseFontIndex + 1) % 3;
      chineseFont = chineseFonts[currentChineseFontIndex];
      InvalidateRect(hwnd, NULL, TRUE);
      return 0;
    case WM_PAINT: {
      PAINTSTRUCT ps;
      HDC hdc = BeginPaint(hwnd, &ps);
      
      // first paragraph
      RECT rect1 = {100, 50, 600, 130};
      drawParagraph(hdc, L"First Paragraph - Bold Arial with Blue Backgorund", &rect1, RGB(0, 0, 255), RGB(0, 0, 0), hFont1);
      
      // second paragraph
      RECT rect2 = {100, 200, 600, 250};
      drawParagraph(hdc, L"Second Paragraph - Itlic Times New Roman with green backgroud and red text", &rect2, RGB(0, 255, 0), RGB(255, 0, 0), hFont2);
      
      // for Chinese characters
      RECT rect3 = {710, 110, 1200, 300};
      HBRUSH borderBrush = CreateSolidBrush(RGB(0, 0, 0)); // black border
      FrameRect(hdc, &rect3, borderBrush);  // Drawing the border

      drawParagraph(hdc, L"驾虹霓，乘赤云，登彼九疑历玉门。\n济天汉，至昆仑，见西王母谒东君。\n交赤松，及羡门，受要秘道爱精神。\n食芝英，饮醴泉，柱杖桂枝佩秋兰。\n绝人事，游浑元，若疾风游欻翩翩。\n景未移，行数千，寿如南山不忘愆。", &rect3, RGB(245, 245, 220), RGB(0, 0, 0), chineseFont);
      DeleteObject(borderBrush);

      //for overlapped rectangles
      SelectObject(hdc, hFont1);
      displatFormattedText(hdc, "Lorem ipsum odor amet, consectetuer adipiscing elit", 300, 300, 200, 100);
      displatFormattedText(hdc, "Curabitur tempus conubia bibendum ex vehicula", 300, 310, 200, 100);

      EndPaint(hwnd, &ps);
      return 0;
    }
  }
  return DefWindowProc(hwnd, message, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
  // register window class
  WNDCLASS wc = {};
  wc.lpfnWndProc = WndProc;
  wc.hInstance = hInstance;
  wc.lpszClassName = "paragraphStyleWindow";
  RegisterClass(&wc);

  // create window
  HWND hWnd = CreateWindowEx(
    0,
    wc.lpszClassName,
    "Play wiht text",
    WS_OVERLAPPEDWINDOW,
    CW_USEDEFAULT,
    CW_USEDEFAULT,
    1500,
    600,
    NULL,
    NULL,
    hInstance,
    NULL
  );

  if (hWnd == NULL) {
    MessageBox(NULL, "Failed to create window", "Error", MB_ICONERROR);
    return 0;
  }

  ShowWindow(hWnd, nCmdShow);

  // run the message loop
  MSG msg = {};
  while (GetMessage(&msg, NULL, 0, 0) > 0) {
    TranslateMessage(&msg);
    DispatchMessage(&msg);
  }

  return 0;
}