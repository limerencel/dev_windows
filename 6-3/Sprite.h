#ifndef SPRITE_H
#define SPRITE_H

#pragma once

#include <windows.h>
#include <string>

class Sprite  {
private:
    int x, y;
    int width, height;
    int frameWidth, frameHeight;
    int currentFrame, maxFrames;
    int frameRow;
    DWORD lastFrameTime, frameDelay;
    bool animate;
    HBITMAP hbmp;
    std::string path;
    COLORREF bitmapBGColor;

public:
    Sprite(int x, int y, COLORREF bitmapBGColor, int framePerRow = 1, bool animate = false) 
    : x(x), y(y), width(0), height(0), hbmp(NULL), path(""), 
    frameWidth(0), frameHeight(0), currentFrame(0), maxFrames(framePerRow), frameRow(0), 
    lastFrameTime(0), frameDelay(100), bitmapBGColor(bitmapBGColor), animate(animate) {};

    ~Sprite() {
        if (hbmp) DeleteObject(hbmp); hbmp = NULL;
    }


    bool LoadBitmap(std::string path) {
        this->path = path;
        hbmp = (HBITMAP)LoadImage(NULL, path.c_str(), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
        if (hbmp == NULL) {
            DWORD error = GetLastError();
            char errorMsg[256];
            FormatMessage(
                FORMAT_MESSAGE_FROM_SYSTEM,
                NULL,
                error,
                0,
                errorMsg,
                256,
                NULL
            );
            MessageBox(NULL, errorMsg, "Error Loading Bitmap", MB_OK | MB_ICONERROR);
            return false;
        } else {
            BITMAP bmp;
            GetObject(hbmp, sizeof(BITMAP), &bmp);
            width = bmp.bmWidth;
            height = bmp.bmHeight;
            if (animate) {
                frameWidth = width / maxFrames;
                frameHeight = height / 4;
            }
            return true;
        }
    }

    void UpdateAnimation() {
        if (animate) {
            DWORD currentTime = GetTickCount();
            if (currentTime - lastFrameTime > frameDelay) {
                currentFrame = (currentFrame + 1) % maxFrames;
                lastFrameTime = currentTime;
            }
        }
    }

    // Getter functions for size of bitmap
    int GetWidth() const { return width; };
    int GetHeight() const { return height; };
    int GetX() const { return x; };
    int GetY() const { return y; };

    void SetX(int x) { this->x = x; };
    void SetPosition(int newX, int newY) { x = newX; y = newY; }

    void SetFrameDelay(DWORD delay) { frameDelay = delay; }
    void SetFrameRow(int row) { frameRow = row % 4; }
    bool IsAnimating() const { return animate; }

    void Render(HDC hdc) {
        if (!hbmp) return;  // Don't render if bitmap isn't loaded
        
        HDC hdcMem = CreateCompatibleDC(hdc);
        HBITMAP hbmpOld = (HBITMAP)SelectObject(hdcMem, hbmp);  // Save old bitmap
        
        TransparentBlt(hdc, x, y, width, height, hdcMem, 0, 0, width, height, bitmapBGColor);
        SelectObject(hdcMem, hbmpOld);  // Restore old bitmap before deleting DC
        DeleteDC(hdcMem);
    }

    void RenderFrame(HDC hdc) {
        if (!hbmp) return;  // Don't render if bitmap isn't loaded
        
        HDC hdcMem = CreateCompatibleDC(hdc);
        HBITMAP hbmpOld = (HBITMAP)SelectObject(hdcMem, hbmp);  // Save old bitmap
        
        TransparentBlt(hdc, x/4, y, frameWidth, frameHeight, hdcMem, currentFrame * frameWidth, frameRow * frameHeight, frameWidth, frameHeight, bitmapBGColor);
        SelectObject(hdcMem, hbmpOld);  // Restore old bitmap before deleting DC
        DeleteDC(hdcMem);
    }

};

#endif // SPRITE_H