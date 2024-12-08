#include <windows.h>

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
	switch (message) {
		case WM_CREATE:
			MessageBox(NULL, "creating", "info", MB_OK | MB_ICONINFORMATION);
			break;
		case WM_PAINT: {
			PAINTSTRUCT ps;
			RECT rect = {100, 100, 500, 500};  // This is your rectangle to draw
			RECT clientRect;
			GetClientRect(hWnd, &clientRect);  // Get the whole window area
			int width = clientRect.right - clientRect.left;
			int height = clientRect.bottom - clientRect.top;
			HDC hdc = BeginPaint(hWnd, &ps);
			// double buffering 
			HDC hdcMem = CreateCompatibleDC(hdc);
			HBITMAP hBmMem = CreateCompatibleBitmap(hdc, width, height);
			HBITMAP hOldBitmap = (HBITMAP)SelectObject(hdcMem, hBmMem);

			// Fill background first
			HBRUSH hBgBrush = (HBRUSH)GetStockObject(WHITE_BRUSH);
			FillRect(hdcMem, &clientRect, hBgBrush);

			// draw a rectangle
			HBRUSH hBrush = CreateSolidBrush(RGB(255,0,0)); // fill the interior
			HBRUSH hOldBrush = (HBRUSH)SelectObject(hdcMem, hBrush);
			HPEN hPen = CreatePen(PS_SOLID, 2, RGB(0,255,0)); //for outline
			HPEN hOldPen = (HPEN)SelectObject(hdcMem, hPen);
			Rectangle(hdcMem, rect.left, rect.top, rect.right, rect.bottom);

			BitBlt(hdc, 0, 0, width, height, hdcMem, 0, 0, SRCCOPY);

			// Cleanup in correct order
			// First restore the original objects
			SelectObject(hdcMem, hOldBrush);
			SelectObject(hdcMem, hOldPen);
			SelectObject(hdcMem, hOldBitmap);
			// Then delete the objects
			DeleteObject(hBrush);
			DeleteObject(hPen);
			DeleteObject(hBmMem);
			DeleteDC(hdcMem);
			EndPaint(hWnd, &ps);
			break;
		}
		case WM_DESTROY:
			PostQuitMessage(0); 
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
	}	
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR pCmdLine, int nCmdShow) {
	WNDCLASS wc = {};
	wc.lpfnWndProc = WndProc;
	wc.hInstance = hInstance;
	wc.lpszClassName = "CLASSNAME";

	RegisterClass(&wc);

	//create window
	HWND hWnd = CreateWindowEx(0, "CLASSNAME", "text", WS_OVERLAPPEDWINDOW,
								CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
								CW_USEDEFAULT,
								NULL, // parent window
								NULL, // Menu
								hInstance, // Instance handle
								NULL // Additional application data
                                );

	if (hWnd == NULL) {
		return 0;
	}

	ShowWindow(hWnd, nCmdShow);
	// UpdateWindow(hWnd);

	MSG msg = {};
	while (GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return 0;

}