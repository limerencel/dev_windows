#include <windows.h>

int left = 50;
int top = 50;
int right = 450;
int bottom = 100;
HBRUSH hBrush;
HWND hEdit;
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
        case WM_CREATE:

            // Create an Edit Control
            hEdit = CreateWindowEx(
                0,
                "EDIT",
                "",
                WS_BORDER | WS_CHILD | WS_VISIBLE | ES_MULTILINE | ES_LEFT,
                left,
                top,
                right - left,
                bottom - top,
                hWnd,
                (HMENU)1,
                ((LPCREATESTRUCT)lParam)->hInstance,
                NULL
            );
            break;
        case WM_DESTROY:
            PostQuitMessage(0);
            break;
        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            FillRect(hdc, &ps.rcPaint, (HBRUSH)(COLOR_WINDOW + 2));
            
            EndPaint(hWnd, &ps);
        }
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    WNDCLASS wc = {};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = "DisplayText";

    RegisterClass(&wc);

    HWND hWnd = CreateWindowEx(
    0,
    wc.lpszClassName,
    "Text Input",
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