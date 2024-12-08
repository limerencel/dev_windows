#pragma once
#include <windows.h>
#include <vector>
#include <random> // Added for random number generation

class Game {
public:
    // Constants
    static const int SQUARE_SIZE = 50;
    static const int BOARD_X = 14;
    static const int BOARD_Y = 8;
    static const int NUMBER_OF_SQUARES = 3;

    // Structures
    struct Position { 
        int x;
        int y;
        bool operator==(const Position& other) const {
            return x == other.x && y == other.y;
        }
    };

    struct Box {
        Position pos;
        bool isLocked;
    };

    struct Destination {
        Position pos;
        bool isOccupied;
    };

    // Static window procedure
    static LRESULT CALLBACK StaticWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
    static Game* instance;  // Single instance of the game

    // Game initialization and run
    static bool Init(HINSTANCE hInstance, int nCmdShow);
    static int Run();

    // Constructor and Destructor
    Game();
    ~Game();

    // Initialization
    bool Initialize(HWND hWnd);

    // Game Logic
    void GenerateNewPositions();
    bool CheckWinCondition() const;
    void HandleKeyDown(WPARAM key);
    void MovePlayer(int dx, int dy);

    // Rendering
    void Render(HDC hdc);
    void SetBoardStartPosition(int x, int y);

private:
    // Helper methods
    Position ScreenToGrid(int screenX, int screenY) const;
    Box* FindBoxAtPosition(const Position& pos);
    Destination* FindDestinationAtPosition(const Position& pos);
    bool IsPositionOccupied(const Position& pos, bool checkDestinations = true) const;
    Position GetRandomUnoccupiedPosition(bool checkDestinations = true);
    bool IsDestination(const Position& pos, bool* isOccupied = nullptr) const;

    // Member variables
    std::vector<Box> boxes;
    std::vector<Destination> destinations;
    Position playerPos;  // Current player position
    int startX, startY;
    HWND hwnd;
    HPEN blackPen;
    HPEN bluePen;
    HBITMAP hBitmap;
    
    // Double buffering
    HDC bufferDC;
    HBITMAP bufferBitmap;
    int windowWidth;
    int windowHeight;

    // Random number generation
    std::random_device rd;
    std::mt19937 gen;
};
