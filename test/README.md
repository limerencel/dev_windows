## Double Buffering
1. Think of a DC (Device Context) as an easel - it's just a structure to hold something to draw on
2. The bitmap is like the actual canvas you put on the easel
    - Without a bitmap, your compatible DC is like an empty easel with nothing to paint on
    - The compatible bitmap creates the actual memory surface where your drawing will be stored

So the process is:

1. `CreateCompatibleDC` → Creates an easel (memory DC)
2. `CreateCompatibleBitmap` → Creates a blank canvas (memory surface)
3. `SelectObject(memoryDC, bitmap)` → Puts the canvas on the easel
4. Draw functions (`Rectangle`, `Ellipse`, etc.) → Paint on this memory canvas
5. `BitBlt` → Copy the finished painting from memory to screen
This is why you need both:

- The compatible DC alone has nowhere to store the pixels
- The bitmap provides the actual memory buffer where the drawing is stored

When you use `BitBlt`, you're essentially saying "take everything that was drawn on this memory bitmap and copy it to the screen in one go" - that's what eliminates the flickering, because all the drawing operations happen in memory first, invisible to the user, and then appear on screen all at once.