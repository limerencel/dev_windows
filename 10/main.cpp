#include <windows.h>
#include <windowsx.h>
#include <vector>
#include <random>
#include <algorithm>

const int SQUARE_SIZE = 50;
const int BOXES_COUNT = 5;
const int BOARD_X = 16;
const int BOARD_Y = 12;

struct Position {
    int x, y;
};

struct Box {
    Position pos;
    bool isSelected;
};

std::vector<Box> boxes;
std::mt19937 gen(std::random_device{}());

HBITMAP hBitmap = NULL;
HDC memDC = NULL;
HBITMAP memBitmap = NULL;
HBITMAP oldMemBitmap = NULL;

Position getRandomPosition() {
    std::uniform_int_distribution<> disX(0, BOARD_X - 1);
    std::uniform_int_distribution<> disY(0, BOARD_Y - 1);
    return {disX(gen), disY(gen)};
}

void initializeBoxes() {
    boxes.clear();
    for (int i = 0; i < BOXES_COUNT; ++i) {
        Box box;
        do {
            box.pos = getRandomPosition();
        } while (std::any_of(boxes.begin(), boxes.end(), [&](const Box& b) { 
            return b.pos.x == box.pos.x && b.pos.y == box.pos.y; 
        }));
        box.isSelected = false;
        boxes.push_back(box);
    }
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
        case WM_CREATE: {
            hBitmap = (HBITMAP)LoadImage(NULL, "box.bmp", IMAGE_BITMAP, SQUARE_SIZE, SQUARE_SIZE, LR_LOADFROMFILE);
            if (hBitmap == NULL) {
                MessageBox(hWnd, "Failed to load bitmap", "Error", MB_OK | MB_ICONERROR);
                return -1;
            }
            initializeBoxes();

            // Set up double buffering
            HDC hdc = GetDC(hWnd);
            memDC = CreateCompatibleDC(hdc);
            RECT clientRect;
            GetClientRect(hWnd, &clientRect);
            memBitmap = CreateCompatibleBitmap(hdc, clientRect.right, clientRect.bottom);
            oldMemBitmap = (HBITMAP)SelectObject(memDC, memBitmap);
            ReleaseDC(hWnd, hdc);

            return 0;
        }       
        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);

            // Get the client area
            RECT clientRect;
            GetClientRect(hWnd, &clientRect);

            // Clear the entire client area (use the window's background color)
            HBRUSH backgroundBrush = (HBRUSH)(COLOR_WINDOW + 1);
            FillRect(memDC, &clientRect, backgroundBrush);

            // Create a compatible DC for the box bitmap
            HDC boxDC = CreateCompatibleDC(memDC);
            HBITMAP oldBoxBitmap = (HBITMAP)SelectObject(boxDC, hBitmap);

            // Create a red pen for selection
            HPEN redPen = CreatePen(PS_SOLID, 2, RGB(255, 0, 0));
            HPEN oldPen = (HPEN)SelectObject(memDC, redPen);
            HBRUSH oldBrush = (HBRUSH)SelectObject(memDC, GetStockObject(NULL_BRUSH));

            // Draw boxes
            for (const auto& box : boxes) {
                int x = box.pos.x * SQUARE_SIZE;
                int y = box.pos.y * SQUARE_SIZE;
                BitBlt(memDC, x, y, SQUARE_SIZE, SQUARE_SIZE, boxDC, 0, 0, SRCCOPY);

                if (box.isSelected) {
                    // Draw the red border
                    Rectangle(memDC, x, y, x + SQUARE_SIZE, y + SQUARE_SIZE);
                }
            }

            // Copy the entire off-screen buffer to the window
            BitBlt(hdc, 0, 0, clientRect.right, clientRect.bottom, memDC, 0, 0, SRCCOPY);

            // Clean up
            SelectObject(memDC, oldPen);
            SelectObject(memDC, oldBrush);
            DeleteObject(redPen);
            SelectObject(boxDC, oldBoxBitmap);
            DeleteDC(boxDC);

            EndPaint(hWnd, &ps);
            return 0;
        }
        case WM_LBUTTONDOWN: {
            int x = GET_X_LPARAM(lParam) / SQUARE_SIZE;
            int y = GET_Y_LPARAM(lParam) / SQUARE_SIZE;
            for (auto& box : boxes) {
                if (box.pos.x == x && box.pos.y == y) {
                    box.isSelected = !box.isSelected;
                    InvalidateRect(hWnd, NULL, TRUE);
                    break;
                }
            }
            return 0;
        }
        case WM_KEYDOWN: {
            if (wParam == VK_SPACE) {
                initializeBoxes();
                InvalidateRect(hWnd, NULL, TRUE);
            } else {
                SetTimer(hWnd, 1, 50, NULL);  // Start a timer that fires every 50ms
            }
            return 0;
        }

        case WM_KEYUP: {
            KillTimer(hWnd, 1);  // Stop the timer when key is released
            return 0;
        }

        case WM_TIMER: {
            int dx = 0, dy = 0;
            if (GetAsyncKeyState(VK_LEFT) & 0x8000)  dx = -1;
            if (GetAsyncKeyState(VK_RIGHT) & 0x8000) dx = 1;
            if (GetAsyncKeyState(VK_UP) & 0x8000)    dy = -1;
            if (GetAsyncKeyState(VK_DOWN) & 0x8000)  dy = 1;
            
            if (dx != 0 || dy != 0) {
                bool moved = false;
                for (auto& box : boxes) {
                    if (box.isSelected) {
                        int newX = box.pos.x + dx;
                        int newY = box.pos.y + dy;
                        if (newX >= 0 && newX < BOARD_X && newY >= 0 && newY < BOARD_Y) {
                            box.pos.x = newX;
                            box.pos.y = newY;
                            moved = true;
                        }
                    }
                }
                if (moved) {
                    // Invalidate the entire board area
                    RECT updateRect = {0, 0, BOARD_X * SQUARE_SIZE, BOARD_Y * SQUARE_SIZE};
                    InvalidateRect(hWnd, &updateRect, FALSE);
                }
            }
            return 0;
        }
        case WM_DESTROY:
            if (hBitmap) DeleteObject(hBitmap);
            if (memDC) {
                SelectObject(memDC, oldMemBitmap);
                DeleteDC(memDC);
            }
            if (memBitmap) DeleteObject(memBitmap);
            PostQuitMessage(0);
            return 0;
    }
    return DefWindowProc(hWnd, message, wParam, lParam);
}
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    WNDCLASSEX wcex = {};
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, IDI_APPLICATION);
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszClassName = "SimpleWindow";
    wcex.hIconSm = LoadIcon(wcex.hInstance, IDI_APPLICATION);

    if (!RegisterClassEx(&wcex)) {
        MessageBox(NULL, "Window Registration Failed!", "Error!", MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

    HWND hWnd = CreateWindowEx(
        0,
        "SimpleWindow",
        "Box Selector",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT,
        BOARD_X * SQUARE_SIZE + 16, BOARD_Y * SQUARE_SIZE + 39,
        NULL, NULL, hInstance, NULL
    );

    if (!hWnd) {
        MessageBox(NULL, "Window Creation Failed!", "Error!", MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return (int)msg.wParam;
}