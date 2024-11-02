#include <windows.h>
#include <random>
#include <vector>

HPEN blackPen;
HPEN bluePen;  // For destinations
HBITMAP hBitmap;
const int SQUARE_SIZE = 50;
const int BOARD_X = 14;
const int BOARD_Y = 8;
const int NUMBER_OF_SQUARES = 3;

// Structure to store grid positions
struct Position {
    int x;
    int y;
};

// Structure to represent a box
struct Box {
    Position pos;
    bool isSelected;
    bool isLocked;
};

// Structure to represent a destination
struct Destination {
    Position pos;
    bool isOccupied;
};

std::vector<Box> boxes;
std::vector<Destination> destinations;
int startX, startY;  // Global variables for board position

// Create a single random number generator to be reused
std::random_device rd;
std::mt19937 gen(rd());

int generateRandomInt(int a, int b) {
    std::uniform_int_distribution<int> dis(a, b);
    return dis(gen);
}

// Check if a position is already occupied by a box or destination
bool isPositionOccupied(const Position& pos, bool checkDestinations = true) {
    // Check boxes
    for (const auto& box : boxes) {
        if (box.pos.x == pos.x && box.pos.y == pos.y) {
            return true;
        }
    }
    
    // Check destinations if requested
    if (checkDestinations) {
        for (const auto& dest : destinations) {
            if (dest.pos.x == pos.x && dest.pos.y == pos.y) {
                return true;
            }
        }
    }
    return false;
}

// Generate a new random unoccupied position
Position getRandomUnoccupiedPosition(bool checkDestinations = true) {
    Position pos;
    do {
        pos.x = generateRandomInt(0, BOARD_X - 1);
        pos.y = generateRandomInt(0, BOARD_Y - 1);
    } while (isPositionOccupied(pos, checkDestinations));
    return pos;
}

// Check if a position is a destination
bool isDestination(const Position& pos, bool* isOccupied = nullptr) {
    for (const auto& dest : destinations) {
        if (dest.pos.x == pos.x && dest.pos.y == pos.y) {
            if (isOccupied) *isOccupied = dest.isOccupied;
            return true;
        }
    }
    return false;
}

// Initialize boxes and destinations with random positions
void generateNewPositions() {
    boxes.clear();
    destinations.clear();
    
    // Create destinations first
    for (int i = 0; i < NUMBER_OF_SQUARES; i++) {
        Destination dest;
        dest.pos = getRandomUnoccupiedPosition(false);
        dest.isOccupied = false;
        destinations.push_back(dest);
    }
    
    // Create boxes
    for (int i = 0; i < NUMBER_OF_SQUARES; i++) {
        Box box;
        box.isSelected = false;
        box.isLocked = false;
        box.pos = getRandomUnoccupiedPosition();
        boxes.push_back(box);
    }
}

// Convert screen coordinates to grid position
Position screenToGrid(int screenX, int screenY) {
    Position pos;
    pos.x = (screenX - startX) / SQUARE_SIZE;
    pos.y = (screenY - startY) / SQUARE_SIZE;
    return pos;
}

// Find box at given grid position
Box* findBoxAtPosition(Position pos) {
    for (auto& box : boxes) {
        if (box.pos.x == pos.x && box.pos.y == pos.y) {
            return &box;
        }
    }
    return nullptr;
}

// Find destination at given grid position
Destination* findDestinationAtPosition(Position pos) {
    for (auto& dest : destinations) {
        if (dest.pos.x == pos.x && dest.pos.y == pos.y) {
            return &dest;
        }
    }
    return nullptr;
}

// Check if all destinations are occupied (game won)
bool checkWinCondition() {
    for (const auto& dest : destinations) {
        if (!dest.isOccupied) {
            return false;
        }
    }
    return true;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
        case WM_CREATE: {
            blackPen = CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
            bluePen = CreatePen(PS_SOLID, 2, RGB(0, 0, 255));
            hBitmap = (HBITMAP)LoadImage(NULL, TEXT("./box.bmp"), IMAGE_BITMAP,
                SQUARE_SIZE, SQUARE_SIZE, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
            if (hBitmap == NULL) {
                MessageBox(hWnd, "Failed to load bitmap", "Error", MB_OK);
                PostQuitMessage(1);
            }
            generateNewPositions();
            break;
        }

        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            RECT rect;
            GetClientRect(hWnd, &rect);
            startX = (rect.right - SQUARE_SIZE*BOARD_X) / 2;
            startY = (rect.bottom - SQUARE_SIZE*BOARD_Y) / 2;

            // Draw grid
            SelectObject(hdc, blackPen);
            for (int col = 0; col <= BOARD_X; col++) {
                MoveToEx(hdc, startX + col * SQUARE_SIZE, startY, NULL);
                LineTo(hdc, startX + col * SQUARE_SIZE, startY + SQUARE_SIZE*BOARD_Y);
            }
            for (int row = 0; row <= BOARD_Y; row++) {
                MoveToEx(hdc, startX, startY + row * SQUARE_SIZE, NULL);
                LineTo(hdc, startX + SQUARE_SIZE*BOARD_X, startY + row * SQUARE_SIZE);
            }

            // Draw destinations
            SelectObject(hdc, bluePen);
            for (const auto& dest : destinations) {
                if (!dest.isOccupied) {  // Only draw unoccupied destinations
                    int destX = startX + dest.pos.x * SQUARE_SIZE;
                    int destY = startY + dest.pos.y * SQUARE_SIZE;
                    Rectangle(hdc, destX, destY, destX + SQUARE_SIZE, destY + SQUARE_SIZE);
                }
            }

            // Draw boxes
            HDC memDC = CreateCompatibleDC(hdc);
            HBITMAP oldBitmap = (HBITMAP)SelectObject(memDC, hBitmap);

            for (const auto& box : boxes) {
                int x = startX + (box.pos.x * SQUARE_SIZE);
                int y = startY + (box.pos.y * SQUARE_SIZE);
                BitBlt(hdc, x, y, SQUARE_SIZE, SQUARE_SIZE, memDC, 0, 0, SRCCOPY);
                
                // Draw highlight for selected box or green for locked box
                if (box.isLocked || box.isSelected) {
                    HPEN highlightPen = CreatePen(PS_SOLID, 2, 
                        box.isLocked ? RGB(0, 255, 0) : RGB(255, 0, 0));
                    HPEN oldPen = (HPEN)SelectObject(hdc, highlightPen);
                    HBRUSH oldBrush = (HBRUSH)SelectObject(hdc, GetStockObject(NULL_BRUSH));
                    Rectangle(hdc, x, y, x + SQUARE_SIZE, y + SQUARE_SIZE);
                    SelectObject(hdc, oldPen);
                    SelectObject(hdc, oldBrush);
                    DeleteObject(highlightPen);
                }
            }

            SelectObject(memDC, oldBitmap);
            DeleteDC(memDC);
            EndPaint(hWnd, &ps);
            
            // Move win condition check outside of WM_PAINT
            if (checkWinCondition()) {
                PostMessage(hWnd, WM_USER + 1, 0, 0);  // Post custom message for win condition
            }
            break;
        }

    // Add a new message handler for the win condition
        case WM_USER + 1: {
            MessageBox(hWnd, "Congratulations! You've won! \n (click ok to start a new game)", "Victory", MB_OK);
            generateNewPositions();  // Start new game
            InvalidateRect(hWnd, NULL, TRUE);
            UpdateWindow(hWnd);  // Force immediate update
            break;
        }
        case WM_LBUTTONDOWN: {
            int mouseX = LOWORD(lParam);
            int mouseY = HIWORD(lParam);
            Position clickPos = screenToGrid(mouseX, mouseY);

            // Deselect all boxes first
            for (auto& box : boxes) {
                box.isSelected = false;
            }

            // Select clicked box if any and not locked
            if (Box* clickedBox = findBoxAtPosition(clickPos)) {
                if (!clickedBox->isLocked) {
                    clickedBox->isSelected = true;
                }
            }

            InvalidateRect(hWnd, NULL, TRUE);
            break;
        }

        case WM_KEYDOWN: {
            // Find selected box
            Box* selectedBox = nullptr;
            for (auto& box : boxes) {
                if (box.isSelected && !box.isLocked) {
                    selectedBox = &box;
                    break;
                }
            }

            if (selectedBox) {
                Position newPos = selectedBox->pos;
                switch (wParam) {
                    case VK_LEFT:  if (newPos.x > 0) newPos.x--; break;
                    case VK_RIGHT: if (newPos.x < BOARD_X - 1) newPos.x++; break;
                    case VK_UP:    if (newPos.y > 0) newPos.y--; break;
                    case VK_DOWN:  if (newPos.y < BOARD_Y - 1) newPos.y++; break;
                    case VK_ESCAPE: PostQuitMessage(0); break;
                }

                // Check if new position is occupied by another box
                if (!findBoxAtPosition(newPos)) {
                    // Move the box
                    selectedBox->pos = newPos;
                    
                    // Check if box is at any destination
                    Destination* dest = findDestinationAtPosition(newPos);
                    if (dest && !dest->isOccupied) {
                        selectedBox->isLocked = true;
                        selectedBox->isSelected = false;
                        dest->isOccupied = true;
                    }
                    
                    InvalidateRect(hWnd, NULL, TRUE);
                }
            }
            else if (wParam == VK_ESCAPE) {
                PostQuitMessage(0);
            }
            break;
        }

        case WM_DESTROY:
            DeleteObject(blackPen);
            DeleteObject(bluePen);
            DeleteObject(hBitmap);
            PostQuitMessage(0);
            break;
    }
    return DefWindowProc(hWnd, message, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    WNDCLASS wc = {};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = "game";
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    RegisterClass(&wc);

    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);

    HWND hWnd = CreateWindowEx(0, wc.lpszClassName, "Game", WS_POPUP,
        0, 0, screenWidth, screenHeight, NULL, NULL, hInstance, NULL);

    ShowWindow(hWnd, nCmdShow);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return 0;
}