#include <windows.h>

#define ID_TIMER 1

// Global Variables
HINSTANCE hInst;
HBITMAP hBitmap = nullptr;
int frameIndex = 0;
int direction = 0;
bool isAnimating = false;

const int frameWidth = 32;
const int frameHeight = 220;

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

void UpdateFrame() {
    frameIndex = (frameIndex + 1) % 4;
}

void DrawFrame(HDC hdc, int x, int y) {
    if (!hBitmap) return;

    HDC hMemDC = CreateCompatibleDC(hdc);
    SelectObject(hMemDC, hBitmap);
    int srcX = frameIndex * frameWidth;
    int srcY = direction * frameHeight;

    BitBlt(hdc, x, y, frameWidth, frameHeight, hMemDC, srcX, srcY, SRCCOPY);
    DeleteDC(hMemDC);
}

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    WNDCLASS wc = {};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = "AnimationWindowClass";

    RegisterClass(&wc);

    HWND hWnd = CreateWindowEx(0,
    "AnimationWindowClass",
    "2D Animation",
    WS_OVERLAPPEDWINDOW,
    CW_USEDEFAULT, CW_USEDEFAULT,
    400, 400,
    nullptr,
    nullptr,
    hInstance,
    nullptr);

    if (!hWnd) {
        return 0;
    }

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    hBitmap = (HBITMAP)LoadImage(
        nullptr, 
        "./walker.bmp", 
        IMAGE_BITMAP, 
        0, 
        0, 
        LR_LOADFROMFILE
    );
    if (!hBitmap) {
        MessageBox(hWnd, "Could not load bitmap!", "Error", MB_OK | MB_ICONERROR);
        return 0;
    }

    SetTimer(hWnd, ID_TIMER, 250, nullptr);

    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    DeleteObject(hBitmap);

    return (int)msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message) {
        case WM_LBUTTONDOWN:
            if (!isAnimating) {
              isAnimating = true;
              SetTimer(hWnd, ID_TIMER, 250, nullptr);
            }
            break;
        case WM_RBUTTONDOWN:
            if (isAnimating) {
              isAnimating = false;
              KillTimer(hWnd, ID_TIMER);
            }
            break;
        case WM_TIMER:
            if (wParam == ID_TIMER && isAnimating) {
                UpdateFrame();
                InvalidateRect(hWnd, nullptr, FALSE);
            }
            break;

        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            DrawFrame(hdc, 50, 50);
            EndPaint(hWnd, &ps);
        }
        break;

        case WM_DESTROY:
            KillTimer(hWnd, ID_TIMER);
            PostQuitMessage(0);
            break;

        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}
