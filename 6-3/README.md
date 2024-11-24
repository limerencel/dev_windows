# Key Difference Between WM_PAINT and Game Loop Approach

1. When Rendering Occurs:
    - WM_PAINT: Renders only when Windows thinks it's necessary (window uncovered, resized, or InvalidateRect called)
    - Game Loop: Renders continuously, regardless of whether the window needs updating
2. Frame Rate Control:
    - WM_PAINT: Relies on Windows messages and timers (like your SetTimer)
    - Game Loop: Direct control over frame rate, can use Sleep or more precise timing
3. Resource Usage:
    - WM_PAINT: More efficient for simple applications, only renders when needed
    - Game Loop: Uses more CPU as it's constantly running, but provides smoother animation
4. Responsiveness:
    - WM_PAINT: Can be less responsive for complex games
    - Game Loop: Better for real-time games, more responsive