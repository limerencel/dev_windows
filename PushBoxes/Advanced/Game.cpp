#include "Game.h"
#include <random>
#include <ctime>

Game* Game::instance = nullptr;

Game::Game() : startX(0), startY(0), hwnd(nullptr), blackPen(nullptr), 
                bluePen(nullptr), hBitmap(nullptr), playerPos{0, 0},
                bufferDC(nullptr), bufferBitmap(nullptr),
                windowWidth(0), windowHeight(0),
                gen(rd()) {
    instance = this;
}

Game::~Game() {
    if (blackPen) DeleteObject(blackPen);
    if (bluePen) DeleteObject(bluePen);
    if (hBitmap) DeleteObject(hBitmap);
    if (bufferBitmap) DeleteObject(bufferBitmap);
    if (bufferDC) DeleteDC(bufferDC);
}

bool Game::Initialize(HWND hWnd) {
    hwnd = hWnd;
    
    // Get window size and calculate board position
    RECT rect;
    GetClientRect(hWnd, &rect);
    windowWidth = rect.right - rect.left;
    windowHeight = rect.bottom - rect.top;
    startX = (windowWidth - SQUARE_SIZE * BOARD_X) / 2;
    startY = (windowHeight - SQUARE_SIZE * BOARD_Y) / 2;
    
    // Create device context and compatible bitmap for double buffering
    HDC hdc = GetDC(hWnd);
    bufferDC = CreateCompatibleDC(hdc);
    bufferBitmap = CreateCompatibleBitmap(hdc, windowWidth, windowHeight);
    SelectObject(bufferDC, bufferBitmap);
    ReleaseDC(hWnd, hdc);
    
    blackPen = CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
    bluePen = CreatePen(PS_SOLID, 2, RGB(0, 0, 255));
    hBitmap = (HBITMAP)LoadImage(NULL, TEXT("./box1.bmp"), IMAGE_BITMAP,
        SQUARE_SIZE, SQUARE_SIZE, LR_LOADFROMFILE | LR_CREATEDIBSECTION);

    if (!hBitmap || !blackPen || !bluePen || !bufferDC || !bufferBitmap) {
        MessageBox(hwnd, "Failed to initialize game resources", "Error", MB_OK);
        return false;
    }

    GenerateNewPositions();
    return true;
}

void Game::GenerateNewPositions() {
    // Reseed the random number generator with current time
    gen.seed(static_cast<unsigned int>(time(nullptr)));
    
    boxes.clear();
    destinations.clear();
    
    // Create destinations first
    for (int i = 0; i < NUMBER_OF_SQUARES; i++) {
        Destination dest;
        dest.pos = GetRandomUnoccupiedPosition(false);
        dest.isOccupied = false;
        destinations.push_back(dest);
    }
    
    // Create boxes
    for (int i = 0; i < NUMBER_OF_SQUARES; i++) {
        Box box;
        box.isLocked = false;
        box.pos = GetRandomUnoccupiedPosition();
        boxes.push_back(box);
    }

    // Set player position last
    playerPos = GetRandomUnoccupiedPosition();
    
    // Force immediate redraw
    InvalidateRect(hwnd, NULL, FALSE);
}

void Game::HandleKeyDown(WPARAM key) {
    switch (key) {
        case VK_LEFT:  MovePlayer(-1, 0); break;
        case VK_RIGHT: MovePlayer(1, 0); break;
        case VK_UP:    MovePlayer(0, -1); break;
        case VK_DOWN:  MovePlayer(0, 1); break;
        case 'R':      GenerateNewPositions(); break;  // Add R key to reset positions
    }
}

void Game::MovePlayer(int dx, int dy) {
    Position newPos = playerPos;
    newPos.x += dx;
    newPos.y += dy;
    
    // Check bounds
    if (newPos.x < 0 || newPos.x >= BOARD_X || newPos.y < 0 || newPos.y >= BOARD_Y)
        return;

    // Check if there's a box at the new position
    Box* box = FindBoxAtPosition(newPos);
    if (box) {
        if (box->isLocked) return;  // Can't move locked boxes

        // Calculate box's new position
        Position newBoxPos = newPos;
        newBoxPos.x += dx;
        newBoxPos.y += dy;
        
        // Check if box can be moved
        if (newBoxPos.x < 0 || newBoxPos.x >= BOARD_X || 
            newBoxPos.y < 0 || newBoxPos.y >= BOARD_Y ||
            FindBoxAtPosition(newBoxPos))
            return;
        
        // Move the box first
        box->pos = newBoxPos;
        
        // Move player
        playerPos = newPos;
        
        // Request redraw without erasing background
        InvalidateRect(hwnd, NULL, FALSE);
        
        // Check if box is on a destination
        bool isOccupied;
        if (IsDestination(newBoxPos, &isOccupied)) {
            box->isLocked = true;
            
            // Update destination status
            for (auto& dest : destinations) {
                if (dest.pos == newBoxPos) {
                    dest.isOccupied = true;
                    break;
                }
            }
            
            // Check win condition after everything is updated
            if (CheckWinCondition()) {
                MessageBox(hwnd, "Congratulations! You won!", "Game Over", MB_OK);
                GenerateNewPositions();
                return;
            }
        }
    } else {
        // Just move player if no box
        playerPos = newPos;
        InvalidateRect(hwnd, NULL, FALSE);
    }
}

Game::Position Game::GetRandomUnoccupiedPosition(bool checkDestinations) {
    // Create distributions for inner board area (excluding borders)
    std::uniform_int_distribution<int> disX(1, BOARD_X - 2);  // 1 to 12 instead of 0 to 13
    std::uniform_int_distribution<int> disY(1, BOARD_Y - 2);  // 1 to 6 instead of 0 to 7
    
    Position pos;
    do {
        pos.x = disX(gen);  // Get random X coordinate (excluding borders)
        pos.y = disY(gen);  // Get random Y coordinate (excluding borders)
    } while (IsPositionOccupied(pos, checkDestinations));
    
    return pos;
}

bool Game::IsPositionOccupied(const Game::Position& pos, bool checkDestinations) const {
    // Check player position
    if (playerPos == pos) return true;

    // Check boxes
    for (const auto& box : boxes) {
        if (box.pos == pos) return true;
    }
    
    // Check destinations if requested
    if (checkDestinations) {
        for (const auto& dest : destinations) {
            if (dest.pos == pos) return true;
        }
    }
    return false;
}

bool Game::IsDestination(const Game::Position& pos, bool* isOccupied) const {
    for (const auto& dest : destinations) {
        if (dest.pos == pos) {
            if (isOccupied) *isOccupied = dest.isOccupied;
            return true;
        }
    }
    return false;
}

void Game::Render(HDC hdc) {
    // Clear the buffer
    RECT rect = {0, 0, windowWidth, windowHeight};
    FillRect(bufferDC, &rect, (HBRUSH)GetStockObject(WHITE_BRUSH));
    
    // Draw grid
    SelectObject(bufferDC, blackPen);
    for (int col = 0; col <= BOARD_X; col++) {
        MoveToEx(bufferDC, startX + col * SQUARE_SIZE, startY, NULL);
        LineTo(bufferDC, startX + col * SQUARE_SIZE, startY + SQUARE_SIZE * BOARD_Y);
    }
    for (int row = 0; row <= BOARD_Y; row++) {
        MoveToEx(bufferDC, startX, startY + row * SQUARE_SIZE, NULL);
        LineTo(bufferDC, startX + SQUARE_SIZE * BOARD_X, startY + row * SQUARE_SIZE);
    }

    // Draw destinations
    SelectObject(bufferDC, bluePen);
    for (const auto& dest : destinations) {
        if (!dest.isOccupied) {
            int destX = startX + dest.pos.x * SQUARE_SIZE;
            int destY = startY + dest.pos.y * SQUARE_SIZE;
            Rectangle(bufferDC, destX, destY, destX + SQUARE_SIZE, destY + SQUARE_SIZE);
        }
    }

    // Draw boxes
    HDC memDC = CreateCompatibleDC(bufferDC);
    HBITMAP oldBitmap = (HBITMAP)SelectObject(memDC, hBitmap);

    for (const auto& box : boxes) {
        int x = startX + (box.pos.x * SQUARE_SIZE);
        int y = startY + (box.pos.y * SQUARE_SIZE);
        BitBlt(bufferDC, x, y, SQUARE_SIZE, SQUARE_SIZE, memDC, 0, 0, SRCCOPY);
        
        if (box.isLocked) {
            HPEN greenPen = CreatePen(PS_SOLID, 2, RGB(0, 255, 0));
            SelectObject(bufferDC, greenPen);
            Rectangle(bufferDC, x, y, x + SQUARE_SIZE, y + SQUARE_SIZE);
            DeleteObject(greenPen);
        }
    }

    // Draw player
    HBRUSH redBrush = CreateSolidBrush(RGB(255, 0, 0));
    HBRUSH oldBrush = (HBRUSH)SelectObject(bufferDC, redBrush);
    int playerX = startX + playerPos.x * SQUARE_SIZE;
    int playerY = startY + playerPos.y * SQUARE_SIZE;
    Ellipse(bufferDC, playerX + 5, playerY + 5, 
            playerX + SQUARE_SIZE - 5, playerY + SQUARE_SIZE - 5);
    SelectObject(bufferDC, oldBrush);
    DeleteObject(redBrush);

    SelectObject(memDC, oldBitmap);
    DeleteDC(memDC);

    // Copy buffer to screen in one operation
    BitBlt(hdc, 0, 0, windowWidth, windowHeight, bufferDC, 0, 0, SRCCOPY);
}

void Game::SetBoardStartPosition(int x, int y) {
    startX = x;
    startY = y;
}

Game::Box* Game::FindBoxAtPosition(const Game::Position& pos) {
    for (auto& box : boxes) {
        if (box.pos == pos) {
            return &box;
        }
    }
    return nullptr;
}

bool Game::CheckWinCondition() const {
    for (const auto& dest : destinations) {
        if (!dest.isOccupied) {
            return false;
        }
    }
    return true;
}

bool Game::Init(HINSTANCE hInstance, int nCmdShow) {
    WNDCLASSEX wc = {0};
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.lpfnWndProc = Game::StaticWndProc;
    wc.hInstance = hInstance;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.lpszClassName = "PushBoxGameWindowClass";

    if (!RegisterClassEx(&wc)) {
        MessageBox(nullptr, "Call to RegisterClassEx failed!", "Error", MB_OK);
        return false;
    }

    HWND hWnd = CreateWindowEx(
        0,
        "PushBoxGameWindowClass",
        "PushBox Game",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT,
        800, 600,
        nullptr,
        nullptr,
        hInstance,
        nullptr
    );

    if (!hWnd) {
        MessageBox(nullptr, "Call to CreateWindow failed!", "Error", MB_OK);
        return false;
    }

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    return true;
}

int Game::Run() {
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return (int)msg.wParam;
}

LRESULT CALLBACK Game::StaticWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
        case WM_CREATE: {
            instance = new Game();
            if (!instance->Initialize(hWnd)) {
                PostQuitMessage(1);
            }
            return 0;
        }

        case WM_ERASEBKGND:
            return 1; // Tell Windows we handled it (prevent flickering)

        case WM_PAINT: {
            if (!instance) return DefWindowProc(hWnd, message, wParam, lParam);
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            instance->Render(hdc);
            EndPaint(hWnd, &ps);
            return 0;
        }

        case WM_KEYDOWN: {
            if (!instance) return DefWindowProc(hWnd, message, wParam, lParam);
            instance->HandleKeyDown(wParam);
            return 0;
        }

        case WM_DESTROY: {
            if (instance) {
                delete instance;
                instance = nullptr;
            }
            PostQuitMessage(0);
            return 0;
        }

        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
    }
}
