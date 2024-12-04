#include "./headers/SpriteManager.h"

SpriteManager::SpriteManager() {}

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

void SpriteManager::MoveWalker(int dx, int dy) {
    auto it = spriteGroups.find("walker");
    if (it == spriteGroups.end() || it->second.instances.empty()) return;

    // Get the first walker sprite (assuming there's only one)
    auto& walker = it->second.instances[0];
    
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
    
    walker->SetPosition(newX, newY);
}
