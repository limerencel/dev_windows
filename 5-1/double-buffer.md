```cpp
case WM_PAINT: {
    PAINTSTRUCT ps;
    HDC hdc = BeginPaint(hWnd, &ps);

    // 1. Create a memory DC compatible with the screen DC
    HDC memDC = CreateCompatibleDC(hdc);

    // 2. Create an offscreen bitmap for double buffering
    HBITMAP offscreenBitmap = CreateCompatibleBitmap(hdc, ps.rcPaint.right - ps.rcPaint.left, ps.rcPaint.bottom - ps.rcPaint.top);
    HBITMAP oldBitmap = (HBITMAP)SelectObject(memDC, offscreenBitmap);

    // 3. Clear the background in the offscreen buffer
    FillRect(memDC, &ps.rcPaint, (HBRUSH)(COLOR_WINDOW + 1));

    // 4. Draw the content to the memory DC
    BITMAP bitmap;
    GetObject(hBitmap, sizeof(BITMAP), &bitmap);
    HDC bitmapDC = CreateCompatibleDC(hdc);
    HBITMAP oldBitmapDC = (HBITMAP)SelectObject(bitmapDC, hBitmap);
    BitBlt(memDC, 0, 0, bitmap.bmWidth, bitmap.bmHeight, bitmapDC, 0, 0, SRCCOPY);

    // 5. Transfer the offscreen buffer to the screen
    BitBlt(hdc, ps.rcPaint.left, ps.rcPaint.top, ps.rcPaint.right - ps.rcPaint.left, ps.rcPaint.bottom - ps.rcPaint.top, memDC, 0, 0, SRCCOPY);

    // 6. Cleanup resources
    SelectObject(bitmapDC, oldBitmapDC);
    DeleteDC(bitmapDC);
    SelectObject(memDC, oldBitmap);
    DeleteObject(offscreenBitmap);
    DeleteDC(memDC);

    EndPaint(hWnd, &ps);
    break;
}
```

# 1. Difference Between BITMAP and HBITMAP

- HBITMAP (Handle to a Bitmap): This is a handle to a GDI (Graphics Device Interface) bitmap object. It’s used to reference a bitmap loaded into memory. You obtain an HBITMAP when you create or load a bitmap, and it serves as a reference to the actual bitmap data in memory.

- BITMAP: This is a structure that contains information about a bitmap, such as its dimensions (width and height), color format, and bits-per-pixel. The BITMAP structure is used to describe the characteristics of the bitmap but does not hold the actual pixel data. You can fill this structure with the GetObject function to retrieve details about an HBITMAP.

# 2. Why Create a CompatibleBitmap?

The CreateCompatibleBitmap function creates a bitmap that is compatible with the device context you pass to it (usually the screen DC or a memory DC). The created bitmap is used as an offscreen drawing surface for double buffering.

While you may think of a bitmap as a file (like a .bmp image), in this context, we are dealing with a bitmap as an in-memory drawing surface. Creating a CompatibleBitmap gives us an offscreen buffer where we can draw graphics before displaying them on the screen. This helps avoid flickering and provides smoother rendering.

# 3. SelectObject(memDC, offscreenBitmap)

Yes, the SelectObject function does indeed "put" the offscreenBitmap into the memDC, making it the drawing surface for the memDC. Any drawing operations performed on memDC will now affect the offscreenBitmap instead of directly affecting the screen. This is a key step for double buffering, where you draw everything to the offscreen bitmap first and then transfer it to the screen.

# 4. GetObject(hBitmap, sizeof(BITMAP), &bitmap)

Yes, you are correct. In this code:
`GetObject(hBitmap, sizeof(BITMAP), &bitmap);`
hBitmap is the HBITMAP loaded in WM_CREATE or elsewhere, representing the bitmap you want to display.
The GetObject function retrieves information about the HBITMAP and stores it in a BITMAP structure. This allows you to access properties like the bitmap's width, height, and color format.

# 5. Why Create Another CompatibleDC Called bitmapDC?

The bitmapDC is created as a **temporary memory DC** to hold the original hBitmap. You use this additional memory DC to prepare the bitmap for copying to the memDC. Essentially, bitmapDC serves as the source for the bitmap data, while memDC is the offscreen buffer where you draw everything before copying to the screen.

# 6. SelectObject(bitmapDC, hBitmap)

This step:
`SelectObject(bitmapDC, hBitmap);`
selects the original hBitmap into bitmapDC, allowing you to perform operations with the bitmap using this DC. This makes bitmapDC the source of the image data when you copy it using BitBlt.

You cannot directly select hBitmap into memDC because hBitmap is treated as a bitmap resource, and you need a memory DC (bitmapDC) as an intermediary to manage the copying.

# 7. Explanation of the Two BitBlt Functions

The two BitBlt calls have distinct roles and together implement the double buffering technique. Here’s how they connect:
First BitBlt:
`BitBlt(memDC, 0, 0, bitmap.bmWidth, bitmap.bmHeight, bitmapDC, 0, 0, SRCCOPY);`
Source: bitmapDC (where hBitmap is selected)
Destination: memDC (the offscreen buffer)
This call copies the bitmap from bitmapDC to the memDC. At this point, the bitmap is drawn onto the offscreen buffer (in memDC).
Second BitBlt:
`BitBlt(hdc, ps.rcPaint.left, ps.rcPaint.top, ps.rcPaint.right - ps.rcPaint.left, ps.rcPaint.bottom - ps.rcPaint.top, memDC, 0, 0, SRCCOPY);`
Source: memDC (the offscreen buffer containing the combined graphics)
Destination: hdc (the actual screen)
This call copies the contents of the offscreen buffer (memDC) to the screen (hdc). This is the final step where the drawn image is displayed to the user.
