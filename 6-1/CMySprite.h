#include <windows.h>
#include <algorithm>
#include <wingdi.h>
#include <cmath>
#include <strsafe.h>


class CMySprite {
private:
    HBITMAP hBitmap;
    BITMAP bitmap;  // Stores bitmap dimensions and color format
    HDC hdcMem;
    int x, y, width, height;
    float scaleFactor;
    COLORREF m_colorKey; // color to be made transparent
    DWORD lastScaleTime;
    DWORD lastMoveTime;
    static const DWORD SCALE_COOLDOWN = 100; // milliseconds
    static const DWORD MOVE_COOLDOWN = 16; // milliseconds
    static const int MOVE_SPEED = 5; // pixels per frame
    bool useTransparency;
    float rotateAngle;
    bool isSelected;
    bool visible = true;
    
public:
    CMySprite(HDC hdc, int x, int y)
        : x(x), y(y), scaleFactor(1.0f), hBitmap(NULL), hdcMem(NULL), lastScaleTime(0), lastMoveTime(0)
        , useTransparency(true), rotateAngle(0.0f), isSelected(false) {
        // ":" intoduces member initializer list, which initializes the member variables
        // with the provided values, bypassing the default initializatioin step.
        HDC hdcScreen = GetDC(NULL); // get the entire screen DC, which is used for off-screen drawing operations
        hdcMem = CreateCompatibleDC(hdcScreen);
        ReleaseDC(NULL, hdcScreen); // now you got what you want-hdcMem
    }


    void LoadBitmap(HINSTANCE hInstance, UINT resourceID) {
        if (hBitmap) DeleteObject(hBitmap);
        hBitmap = (HBITMAP)LoadImage(hInstance, MAKEINTRESOURCE(resourceID), IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION);
        if (hBitmap == NULL) {
            MessageBox(NULL, "Error loading bitmap", "Error", MB_OK | MB_ICONERROR);
            return;
        }
        GetObject(hBitmap, sizeof(bitmap), &bitmap);
        width = bitmap.bmWidth;
        height = bitmap.bmHeight;
        SelectObject(hdcMem, hBitmap);
    }

    ~CMySprite() {
        if (hBitmap) DeleteObject(hBitmap);
        if (hdcMem) DeleteDC(hdcMem);
    }

    // Methods for scaling
    void increaseScale() {
        DWORD currentTime = GetTickCount();
        if (currentTime - lastScaleTime < SCALE_COOLDOWN) return;
        scaleFactor *= 1.1f; // Increase by 10%
        scaleFactor = min(scaleFactor, 3.0f); // Cap at 200% of original size
        lastScaleTime = currentTime;
    }

    void decreaseScale() {
        DWORD currentTime = GetTickCount();
        if (currentTime - lastScaleTime < SCALE_COOLDOWN) return;
        scaleFactor *= 0.9f; // Decrease by 10%
        scaleFactor = max(scaleFactor, 0.5f); // Don't go below 50% of original size
        lastScaleTime = currentTime;
    }

    void move(int dx, int dy) {
        DWORD currentTime = GetTickCount();
        if (currentTime - lastMoveTime < MOVE_COOLDOWN) return;
        x += dx * MOVE_SPEED;
        y += dy * MOVE_SPEED;
        lastMoveTime = currentTime;
    }

    void rotate(float degrees) {
        rotateAngle += degrees;
        rotateAngle = std::fmod(rotateAngle, 360.0f);
        char debug[100];
        StringCbPrintfA(debug, sizeof(debug), "Rotate called. New angle: %f\n", rotateAngle);
        OutputDebugStringA(debug);
    }

    bool isColliding(const CMySprite* other) {
        RECT thisRect = { x, y, x + getScaledWidth(), y + getScaledHeight() };
        RECT otherRect = { other->x, other->y, other->x + other->getScaledWidth(), other->y + getScaledHeight() };
        RECT intersection;
        return IntersectRect(&intersection, &thisRect, &otherRect);
    }
    bool isVisible() const { return visible; }
    void setVisible(bool isVisible) { visible = isVisible; };

    int getArea() const { return getScaledWidth() * getScaledHeight(); }


    void constrainToScreen(int screenWidth, int screenHeight) {
        int spriteWidth = getScaledWidth();
        int spriteHeight = getScaledHeight();

        x = max(0, min(x, screenWidth - spriteWidth));
        y = max(0, min(y, screenHeight - spriteHeight));
    }

    bool containsPoint(int px, int py) const {
        return px >= x && px < x + getScaledWidth() && py >= y && py < y + getScaledHeight();
    }

    void setSelected(bool selected) { isSelected = selected; }
    bool getSelected() const { return isSelected; }

    int getScaledWidth() const { return static_cast<int>(width * scaleFactor); }
    int getScaledHeight() const { return static_cast<int>(height * scaleFactor); }

    void SetColorKey(COLORREF color) { m_colorKey = color; }

    void setTransparency(bool transparent) { useTransparency = transparent; }
    COLORREF GetColorKey() const { return m_colorKey; }
    void Render(HDC hdcDest) {
        if (hBitmap) {
            int scaledWidth = getScaledWidth();
            int scaledHeight = getScaledHeight();
            if (rotateAngle != 0.0f) {
                // Save the original state
                int oldGraphicsMode = SetGraphicsMode(hdcDest, GM_ADVANCED);
                XFORM xform;
                float radians = rotateAngle * 3.14159f / 180.0f;

                // Set rotation transform matrix
                xform.eM11 = cos(radians);
                xform.eM12 = sin(radians);
                xform.eM21 = -sin(radians);
                xform.eM22 = cos(radians);
                xform.eDx = x + scaledWidth / 2;
                xform.eDy = y + scaledHeight / 2;

                SetWorldTransform(hdcDest, &xform);

                // Draw the sprite centered at (0, 0) in rotated coordinate space
                if (useTransparency) {
                    TransparentBlt(hdcDest, -scaledWidth / 2, -scaledHeight / 2, scaledWidth, scaledHeight,
                        hdcMem, 0, 0, width, height, m_colorKey);
                }
                else {
                    StretchBlt(hdcDest, -scaledWidth / 2, -scaledHeight / 2, scaledWidth, scaledHeight,
                        hdcMem, 0, 0, width, height, SRCCOPY);
                }

                // Restore graphics mode and transformation
                ModifyWorldTransform(hdcDest, NULL, MWT_IDENTITY); // Reset transformation
                SetGraphicsMode(hdcDest, oldGraphicsMode);
            }
            else {
                if (useTransparency) {
                    TransparentBlt(hdcDest, x, y, scaledWidth, scaledHeight, hdcMem, 0, 0, width, height, m_colorKey);
                }
                else {
                    StretchBlt(hdcDest, x, y, scaledWidth, scaledHeight, hdcMem, 0, 0, width, height, SRCCOPY);
                }
            }
        }
    }

    // Get position for boundary checking
    POINT getPosition() { return { x, y }; }
    void setPosition(int newX, int newY) { x = newX; y = newY; }
};
