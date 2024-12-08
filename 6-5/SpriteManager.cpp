#include "./headers/SpriteManager.h"

SpriteManager::SpriteManager() : 
    patrolLeft(300), 
    patrolRight(WINDOW_WIDTH - 800),
    patrolTop(100),
    // Account for full sprite bitmap height (512px) even though only one frame is visible
    // Window height (800) - sprite height (512) ≈ 288px usable space
    patrolBottom(WINDOW_HEIGHT - 600),
    patrolState(0),
    isPatrolling(false) {
}

bool SpriteManager::CreateSpriteType(const std::string& typeName, int x, int y, 
                    COLORREF bgColor, int maxInstances,
                    int framesPerRow, bool animate) {
    if (spriteGroups.find(typeName) != spriteGroups.end()) {
        return false; // Type already exists
    }

    SpriteGroup group;
    group.maxInstances = maxInstances;
    group.currentCount = 0;
    group.x = x;
    group.y = y;
    group.bgColor = bgColor;
    group.framesPerRow = framesPerRow;
    group.animate = animate;
    spriteGroups[typeName] = std::move(group);
    return true;

}

bool SpriteManager::LoadSpriteBitmap(const std::string& typeName, const std::string& bitmapPath) {
    auto it = spriteGroups.find(typeName);
    if (it == spriteGroups.end()) {
        return false;
    }
    
    // Store the bitmap path for later use when creating new sprites
    it->second.bitmapPath = bitmapPath;
    return true;
}

bool SpriteManager::AddInstance(const std::string& typeName, int x, int y) {
    auto it = spriteGroups.find(typeName);
    if (it == spriteGroups.end()) return false;

    auto& group = it->second;
    if (group.currentCount >= group.maxInstances) return false;

    // Clone properties from stored creation parameters
    auto newSprite = std::make_unique<Sprite>(group.x, group.y, group.bgColor, group.framesPerRow, group.animate);
    
    // Clamp position within window bounds using min/max
    x = std::max(0, std::min(x, WINDOW_WIDTH - newSprite->GetWidth()));
    y = std::max(0, std::min(y, WINDOW_HEIGHT - newSprite->GetHeight()));
    
    newSprite->SetX(x);
    newSprite->SetY(y);
    
    // Load the bitmap if a path is stored
    if (!group.bitmapPath.empty()) {
        newSprite->LoadBitmap(group.bitmapPath);
    }
    
    group.instances.push_back(std::move(newSprite));
    group.currentCount++;
    return true;
}

void SpriteManager::UpdateAll() {
    for (auto& pair : spriteGroups) {
        auto& group = pair.second;
        for (auto& sprite : group.instances) {
            sprite->UpdateAnimation();
        }
    }
}

void SpriteManager::RenderAll(HDC hdc) {
    for (auto& pair : spriteGroups) {
        auto& group = pair.second;
        for (auto& sprite : group.instances) {
            // Always use RenderFrame for walker, use Render/RenderFrame for others based on animation state
            if (pair.first == "walker" || sprite->IsAnimating()) {
                sprite->RenderFrame(hdc);
            } else {
                sprite->Render(hdc);
            }
        }
    }
}

bool SpriteManager::HasSpace(const std::string& typeName) const {
    auto it = spriteGroups.find(typeName);
    if (it == spriteGroups.end()) return false;
    return it->second.currentCount < it->second.maxInstances;
}

bool SpriteManager::CheckCollision(const Sprite* sprite1, const Sprite* sprite2) {
    // Calculate the actual frame size (assuming 4x4 grid of frames)
    int frameWidth = sprite1->GetWidth() / 4;
    int frameHeight = sprite1->GetHeight() / 4;
    
    // Get positions
    int x1 = sprite1->GetX();
    int y1 = sprite1->GetY();
    int x2 = sprite2->GetX();
    int y2 = sprite2->GetY();
    
    // Create collision boxes using frame size instead of full sprite size
    RECT rect1 = { x1, y1, x1 + frameWidth, y1 + frameHeight };
    RECT rect2 = { x2, y2, x2 + frameWidth, y2 + frameHeight };
    
    // Check if rectangles overlap
    return !(rect1.right < rect2.left || 
            rect1.left > rect2.right || 
            rect1.bottom < rect2.top || 
            rect1.top > rect2.bottom);
}

void SpriteManager::MoveWalker(int dx, int dy) {
    auto walkerIt = spriteGroups.find("walker");
    auto monsterIt = spriteGroups.find("monster");
    auto boxIt = spriteGroups.find("box");
    if (walkerIt == spriteGroups.end() || walkerIt->second.instances.empty()) return;

    // Get the first walker sprite (assuming there's only one)
    auto& walker = walkerIt->second.instances[0];
    
    // Calculate new position
    int newX = walker->GetX() + dx;
    int newY = walker->GetY() + dy;
    
    // Keep sprite within window bounds
    newX = std::max(0, std::min(newX, WINDOW_WIDTH - walker->GetWidth()));
    newY = std::max(0, std::min(newY, WINDOW_HEIGHT - walker->GetHeight()));
    
    // Only animate and change direction if actually moving
    if (dx != 0 || dy != 0) {
        // Enable animation if moving
        walker->SetAnimate(true);
        
        // Set animation row based on movement direction
        if (dx < 0) {
            walker->SetFrameRow(1);      // Left
        } else if (dx > 0) {
            walker->SetFrameRow(2);      // Right
        } else if (dy < 0) {
            walker->SetFrameRow(3);      // Up
        } else if (dy > 0) {
            walker->SetFrameRow(0);      // Down
        }
    } else {
        // Disable animation when not moving
        walker->SetAnimate(false);
    }
    
    // Update position
    walker->SetPosition(newX, newY);
    
    // Check collision with box
    if (boxIt != spriteGroups.end() && !boxIt->second.instances.empty()) {
        auto& box = boxIt->second.instances[0];
        if (CheckCollision(walker.get(), box.get())) {
            // Store box position before removing it
            int boxX = box->GetX();
            int boxY = box->GetY();
            
            // Remove the box
            boxIt->second.instances.clear();
            boxIt->second.currentCount = 0;
            
            // Add apple at the same position
            auto appleIt = spriteGroups.find("apple");
            if (appleIt != spriteGroups.end() && appleIt->second.currentCount == 0) {
                // Only add apple if the sprite type exists and there isn't already an apple
                if (AddInstance("apple", boxX, boxY)) {
                    // Successfully added apple
                    appleIt->second.instances[0]->SetPosition(boxX, boxY);
                }
            }
        }
    }
    
    // Check collision with monster
    if (monsterIt != spriteGroups.end() && !monsterIt->second.instances.empty()) {
        auto& monster = monsterIt->second.instances[0];
        if (CheckCollision(walker.get(), monster.get())) {
            // Remove the walker when it collides with monster
            walkerIt->second.instances.clear();
            walkerIt->second.currentCount = 0;
            return;  // Exit the function since walker is now gone
        }
    }
}

// Monster Patrol
void SpriteManager::MonsterPatrol(int dx, int dy) {
    auto it = spriteGroups.find("monster");
    if (it == spriteGroups.end() || it->second.instances.empty()) return;

    auto& monster = it->second.instances[0];
    int currentX = monster->GetX();
    int currentY = monster->GetY();
    int moveSpeed = 5;  // Adjust this value to control movement speed

    // Update position based on patrol state
    switch (patrolState) {
        case 0:  // Moving right
            dx = moveSpeed;
            dy = 0;
            if (currentX + dx >= patrolRight) {
                patrolState = 1;  // Change to moving down
                dx = 0;  // Stop horizontal movement
                dy = moveSpeed;  // Start vertical movement
            }
            break;
        case 1:  // Moving down
            dx = 0;
            dy = moveSpeed;
            if (currentY + dy >= patrolBottom) {
                patrolState = 2;  // Change to moving left
                dy = 0;  // Stop vertical movement
                dx = -moveSpeed;  // Start moving left
            }
            break;
        case 2:  // Moving left
            dx = -moveSpeed;
            dy = 0;
            if (currentX + dx <= patrolLeft) {
                patrolState = 3;  // Change to moving up
                dx = 0;  // Stop horizontal movement
                dy = -moveSpeed;  // Start moving up
            }
            break;
        case 3:  // Moving up
            dx = 0;
            dy = -moveSpeed;
            if (currentY + dy <= patrolTop) {
                patrolState = 0;  // Change to moving right
                dy = 0;  // Stop vertical movement
                dx = moveSpeed;  // Start moving right
            }
            break;
    }

    // Calculate new position
    int newX = currentX + dx;
    int newY = currentY + dy;

    // Keep sprite within window bounds
    newX = std::max(0, std::min(newX, WINDOW_WIDTH - monster->GetWidth()));
    newY = std::max(0, std::min(newY, WINDOW_HEIGHT - monster->GetHeight()));
    
    // Update animation frame based on movement direction
    monster->SetAnimate(true);  // Always animate when patrolling
    if (dx < 0) {
        monster->SetFrameRow(1);      // Left
    } else if (dx > 0) {
        monster->SetFrameRow(2);      // Right
    } else if (dy < 0) {
        monster->SetFrameRow(3);      // Up
    } else if (dy > 0) {
        monster->SetFrameRow(0);      // Down
    }
    
    monster->SetPosition(newX, newY);
}