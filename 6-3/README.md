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


# What is std::make_unique?
std::make_unique is a C++ standard library function that creates a unique_ptr (smart pointer) to manage dynamically allocated objects
`auto newSprite = std::make_unique<Sprite>(group.x, group.y, group.bgColor, group.framesPerRow, group.animate);`
It Creates a new Sprite object on the heap and returns a smart pointer that will automatically delete the sprite when it's no longer needed.

# What is std::move?
std::move is used to indicate that an object can be "moved from" - meaning its resources can be transferred to another object. This is particularly important with unique_ptr because:
1. unique_ptr cannot be copied (it ensures single ownership)
2. std::move allows transferring ownership from one unique_ptr to another

Example in SpriteManager:
```cpp
auto newSprite = std::make_unique<Sprite>(...);        // newSprite owns the Sprite
group.instances.push_back(std::move(newSprite));       // ownership moves to instances vector
// After this, newSprite becomes null as it no longer owns anything
```
Think of it like moving furniture:
- Without move: You'd need to make a copy of your couch (expensive/impossible)
- With move: You just transfer the couch to its new location (efficient)

Visual representation of std::move with unique_ptr:

Before std::move:
newSprite (unique_ptr) -----> [Sprite object in heap]
instances vector []

After std::move:
newSprite (unique_ptr) -----> nullptr
instances vector [unique_ptr -----> [Sprite object in heap]]

The Sprite object stays in the same place in memory - only the ownership (who's responsible for managing it) changes.