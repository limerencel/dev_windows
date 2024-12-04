#ifndef SPRITE_MANAGER_H
#define SPRITE_MANAGER_H

#pragma once
#include "Sprite.h"
#include "WindowConfig.h"
#include <vector>
#include <memory>
#include <string>
#include <unordered_map>

class SpriteManager {
private:
    struct SpriteGroup {
        std::vector<std::unique_ptr<Sprite>> instances;
        int maxInstances;
        int currentCount;
        int x;
        int y;
        COLORREF bgColor;
        int framesPerRow;
        bool animate;
        std::string bitmapPath; // Added to store the bitmap path
    };

    std::unordered_map<std::string, SpriteGroup> spriteGroups;
    Sprite* selectedSprite;  // Pointer to currently selected sprite (non-owning)

public:
    SpriteManager() : selectedSprite(nullptr) {}

    bool CreateSpriteType(const std::string& typeName, int x, int y, 
                        COLORREF bgColor, int maxInstances = 10,
                        int framesPerRow = 1, bool animate = false) {
        if (spriteGroups.find(typeName) != spriteGroups.end()) {
            return false; // Type already exists
        }

        SpriteGroup group;
        group.maxInstances = maxInstances;
        group.currentCount = 0;  // Start with 0 sprites
        
        // Store the sprite creation parameters for later use
        group.x = x;
        group.y = y;
        group.bgColor = bgColor;
        group.framesPerRow = framesPerRow;
        group.animate = animate;
        
        spriteGroups[typeName] = std::move(group);
        return true;
    }

    bool LoadSpriteBitmap(const std::string& typeName, const std::string& bitmapPath) {
        auto it = spriteGroups.find(typeName);
        if (it == spriteGroups.end()) {
            return false;
        }
        
        // Store the bitmap path for later use when creating new sprites
        it->second.bitmapPath = bitmapPath;
        return true;
    }

    bool AddInstance(const std::string& typeName) {
        auto it = spriteGroups.find(typeName);
        if (it == spriteGroups.end()) return false;

        auto& group = it->second;
        if (group.currentCount >= group.maxInstances) return false;

        // Clone properties from stored creation parameters
        auto newSprite = std::make_unique<Sprite>(group.x, group.y, group.bgColor, group.framesPerRow, group.animate);
        
        // Generate random positions within window bounds, accounting for sprite size
        int randomX = rand() % (WINDOW_WIDTH - newSprite->GetWidth());
        int randomY = rand() % (WINDOW_HEIGHT - newSprite->GetHeight());
        
        newSprite->SetX(randomX);
        newSprite->SetY(randomY);
        
        // Load the bitmap if a path is stored
        if (!group.bitmapPath.empty()) {
            newSprite->LoadBitmap(group.bitmapPath);
        }
        
        group.instances.push_back(std::move(newSprite));
        group.currentCount++;
        return true;
    }

    void UpdateAll() {
        for (auto& pair : spriteGroups) {
            auto& group = pair.second;
            for (auto& sprite : group.instances) {
                sprite->UpdateAnimation();
            }
        }
    }

    void RenderAll(HDC hdc) {
        for (auto& pair : spriteGroups) {
            auto& group = pair.second;
            for (auto& sprite : group.instances) {
                if (sprite->IsAnimating()) {
                    sprite->RenderFrame(hdc);
                } else {
                    sprite->Render(hdc);
                }
            }
        }
    }

    bool HasSpace(const std::string& typeName) const {
        auto it = spriteGroups.find(typeName);
        if (it == spriteGroups.end()) return false;
        return it->second.currentCount < it->second.maxInstances;
    }

    // Bring the selected sprite to the front
    void BringSelectedSpriteToFront() {
        if (!selectedSprite) return;

        // Find the sprite in its group
        for (auto& pair : spriteGroups) {
            auto& group = pair.second;
            for (auto it = group.instances.begin(); it != group.instances.end(); ++it) {
                if (it->get() == selectedSprite) {
                    // Move this sprite to the end of the vector (top of render order)
                    auto sprite = std::move(*it);
                    group.instances.erase(it);
                    group.instances.push_back(std::move(sprite));
                    return;
                }
            }
        }
    }

    // Select sprite at given coordinates, returns true if a sprite was selected
    bool SelectSpriteAt(int x, int y) {
        // First, deselect current sprite if any
        if (selectedSprite) {
            selectedSprite->SetSelected(false);
            selectedSprite = nullptr;
        }

        // Check each sprite group
        for (auto& pair : spriteGroups) {
            auto& group = pair.second;
            // Check sprites in reverse order (top to bottom)
            for (auto it = group.instances.rbegin(); it != group.instances.rend(); ++it) {
                if ((*it)->ContainsPoint(x, y)) {
                    selectedSprite = it->get();
                    selectedSprite->SetSelected(true);
                    BringSelectedSpriteToFront(); // Bring the selected sprite to front
                    return true;
                }
            }
        }
        return false;
    }

    // Move selected sprite to new position
    void MoveSelectedSprite(int x, int y) {
        if (selectedSprite) {
            // Keep sprite within window bounds
            x = std::max(0, std::min(x, WINDOW_WIDTH - selectedSprite->GetWidth()));
            y = std::max(0, std::min(y, WINDOW_HEIGHT - selectedSprite->GetHeight()));
            
            selectedSprite->SetPosition(x, y);
        }
    }

    // Move selected sprite by a relative amount
    void MoveSelectedSpriteBy(int dx, int dy) {
        if (selectedSprite) {
            int newX = selectedSprite->GetX() + dx;
            int newY = selectedSprite->GetY() + dy;
            
            // Keep sprite within window bounds
            newX = std::max(0, std::min(newX, WINDOW_WIDTH - selectedSprite->GetWidth()));
            newY = std::max(0, std::min(newY, WINDOW_HEIGHT - selectedSprite->GetHeight()));
            
            selectedSprite->SetPosition(newX, newY);
        }
    }

    // Deselect current sprite
    void DeselectSprite() {
        if (selectedSprite) {
            selectedSprite->SetSelected(false);
            selectedSprite = nullptr;
        }
    }
};

#endif // SPRITE_MANAGER_H
