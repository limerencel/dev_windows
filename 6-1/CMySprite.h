#include <windows.h>
#include <algorithm>
#include <wingdi.h>
#include <cmath>
#include <iostream>

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

    void RenderRotated(HDC hdcDest, int scaledWidth, int scaledHeight) {
        char debug[100];
        sprintf(debug, "RenderRotated called. Angle: %f\n", rotateAngle);
        OutputDebugString(debug);

        // Create a temporary DC and bitmap for rotation
        HDC hdcTemp = CreateCompatibleDC(hdcDest);
        HBITMAP hbmTemp = CreateCompatibleBitmap(hdcDest, scaledWidth, scaledHeight);
        HBITMAP hbmOld = (HBITMAP)SelectObject(hdcTemp, hbmTemp);

        // Set the background of temporary DC to be transparent
        SetBkMode(hdcTemp, TRANSPARENT);

        // Create a transformation matrix for rotation
        XFORM xform;
        float radians = rotateAngle * 3.14159f / 180.0f;  // Convert degrees to radians
        float cosTheta = cos(radians);
        float sinTheta = sin(radians);
        xform.eM11 = cosTheta;
        xform.eM12 = sinTheta;
        xform.eM21 = -sinTheta;
        xform.eM22 = cosTheta;
        xform.eDx = scaledWidth / 2.0f;
        xform.eDy = scaledHeight / 2.0f;

        // Set the graphics mode to advanced and apply the transformation
        int oldGraphicsMode = SetGraphicsMode(hdcTemp, GM_ADVANCED);
        SetWorldTransform(hdcTemp, &xform);

        // Draw the rotated sprite onto the temporary DC
        if (useTransparency) {
            TransparentBlt(hdcTemp, -scaledWidth/2, -scaledHeight/2, scaledWidth, scaledHeight, 
                hdcMem, 0, 0, width, height, m_colorKey);
        } else {
            StretchBlt(hdcTemp, -scaledWidth/2, -scaledHeight/2, scaledWidth, scaledHeight, 
            hdcMem, 0, 0, width, height, SRCCOPY);
        }

        // Reset the graphics mode
        SetGraphicsMode(hdcTemp, oldGraphicsMode);

        // Copy the rotated sprite to the destination DC
        BitBlt(hdcDest, x, y, scaledWidth, scaledHeight, hdcTemp, 0, 0, SRCCOPY);

        // Clean up
        SelectObject(hdcTemp, hbmOld);
        DeleteObject(hbmTemp);
        DeleteDC(hdcTemp);
    }

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


    void LoadBitmap(const char* filename) {
        if (hBitmap) DeleteObject(hBitmap);
        hBitmap = (HBITMAP)LoadImage(NULL, filename, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
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
        scaleFactor = std::min(scaleFactor, 2.0f); // Cap at 200% of original size
        lastScaleTime = currentTime;
    }

    void decreaseScale() {
        DWORD currentTime = GetTickCount();
        if (currentTime - lastScaleTime < SCALE_COOLDOWN) return;
        scaleFactor *= 0.9f; // Decrease by 10%
        scaleFactor = std::max(scaleFactor, 0.5f); // Don't go below 50% of original size
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
        sprintf(debug, "Rotate called. New angle: %f\n", rotateAngle);
        OutputDebugString(debug);
    }

    void constrainToScreen(int screenWidth, int screenHeight) {
    int spriteWidth = getScaledWidth();
    int spriteHeight = getScaledHeight();

    x = std::max(0, std::min(x, screenWidth - spriteWidth));
    y = std::max(0, std::min(y, screenHeight - spriteHeight));
    }

    bool containsPoint(int px, int py) const {
        return px >= x && px < x + getScaledWidth() && py >= y && py < y + getScaledHeight();
    }

    void setSelected(bool selected) {isSelected = selected;}
    bool getSelected() const {return isSelected;}

    int getScaledWidth() const { return static_cast<int>(width * scaleFactor); }
    int getScaledHeight() const { return static_cast<int>(height * scaleFactor); }

    void SetColorKey(COLORREF color) {m_colorKey = color;}

    void setTransparency(bool transparent) {useTransparency = transparent;}
    COLORREF GetColorKey() const {return m_colorKey;}
    void Render(HDC hdcDest) {
        if (hBitmap) {
            int scaledWidth = getScaledWidth();
            int scaledHeight = getScaledHeight();
            if (rotateAngle!= 0.0f) {
                RenderRotated(hdcDest, scaledWidth, scaledHeight);
            } else {
                if (useTransparency) {
                    TransparentBlt(hdcDest, x, y, scaledWidth, scaledHeight, hdcMem, 0, 0, width, height, m_colorKey);
                } else {
                    StretchBlt(hdcDest, x, y, scaledWidth, scaledHeight, hdcMem, 0, 0, width, height, SRCCOPY);
                }
            }
        }
    }

    // Get position for boundary checking
    POINT getPosition() {return {x, y};}
    void setPosition(int newX, int newY) {x = newX; y = newY; }
};
