#include <windows.h>

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
void DrawShape(HDC hdc, RECT rect, bool isEllipse);

POINT startPoint;       // Start point of the drag
RECT currentRect;       // Current rectangle being drawn
bool isDrawing = false; // Flag for drawing state

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_HREDRAW | CS_VREDRAW, WndProc, 0, 0, hInstance, NULL, LoadCursor(NULL, IDC_ARROW), (HBRUSH)(COLOR_WINDOW+1), NULL, TEXT("DrawShapes"), NULL };
    RegisterClassEx(&wc);

    HWND hwnd = CreateWindow(wc.lpszClassName, TEXT("Rectangle and Ellipse Drawing"), WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 800, 600, NULL, NULL, hInstance, NULL);
    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return (int)msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_LBUTTONDOWN: {
            // Start drawing
            isDrawing = true;
            startPoint.x = LOWORD(lParam);
            startPoint.y = HIWORD(lParam);
            currentRect = { startPoint.x, startPoint.y, startPoint.x, startPoint.y };
            InvalidateRect(hwnd, NULL, TRUE);
            break;
        }
        case WM_MOUSEMOVE: {
            if (isDrawing) {
                // Update the current rectangle
                int x = LOWORD(lParam);
                int y = HIWORD(lParam);
                currentRect.right = x;
                currentRect.bottom = y;

                // Redraw the window
                InvalidateRect(hwnd, NULL, TRUE);
            }
            break;
        }
        case WM_LBUTTONUP: {
            if (isDrawing) {
                // End drawing
                isDrawing = false;
                // InvalidateRect(hwnd, NULL, TRUE);
            }
            break;
        }
        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);

            if (isDrawing) {
                // Draw the current shape
                bool isEllipse = (GetKeyState(VK_SHIFT) & 0x8000) != 0;
                DrawShape(hdc, currentRect, isEllipse);
            }

            EndPaint(hwnd, &ps);
            break;
        }
        case WM_DESTROY:
            PostQuitMessage(0);
            break;
        default:
            return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}

void DrawShape(HDC hdc, RECT rect, bool isEllipse) {
    // Draw rectangle or ellipse depending on isEllipse flag
    if (isEllipse) {
        Ellipse(hdc, rect.left, rect.top, rect.right, rect.bottom);
    } else {
        Rectangle(hdc, rect.left, rect.top, rect.right, rect.bottom);
    }
}
