#ifndef SPRITE_MANAGER_LIST_H
#define SPRITE_MANAGER_LIST_H

#pragma once
#include "Sprite.h"
#include "WindowConfig.h"
#include <list>
#include <memory>
#include <string>
#include <unordered_map>

class SpriteManagerList {
private:
    struct SpriteGroup {
        std::list<std::unique_ptr<Sprite>> instances;  // Changed from vector to list
        int maxInstances;
        int currentCount;
        int x;
        int y;
        COLORREF bgColor;
        int framesPerRow;
        bool animate;
        std::string bitmapPath;
    };

    std::unordered_map<std::string, SpriteGroup> spriteGroups;
    Sprite* selectedSprite;  // Pointer to currently selected sprite (non-owning)

public:
    SpriteManagerList() : selectedSprite(nullptr) {}

    // create a new sprite type
    bool CreateSpriteType(const std::string& typeName, int x, int y, 
                    COLORREF bgColor, int maxInstances = 10,
                    int framesPerRow = 1, bool animate = false) {
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

    // Load a bitmap for a sprite type
    bool LoadSpriteBitmap(const std::string& typeName, const std::string& bitmapPath) {
        auto it = spriteGroups.find(typeName);
        if (it == spriteGroups.end()) {
            return false;
        }
        
        // Store the bitmap path for later use when creating new sprites
        it->second.bitmapPath = bitmapPath;
        return true;
    }

    // add an instance of a sprite type
    bool AddInstance(const std::string& typeName) {
        auto it = spriteGroups.find(typeName);
        if (it == spriteGroups.end()) return false;

        if (it->second.currentCount < it->second.maxInstances) {
            auto newSprite = new Sprite(it->second.x, it->second.y, 
            it->second.bgColor, it->second.framesPerRow, it->second.animate);

            // generate random positions within window bounds, accounting for sprite size
            int randomX = rand() % (WINDOW_WIDTH - newSprite->GetWidth());
            int randomY = rand() % (WINDOW_HEIGHT - newSprite->GetHeight());
            
            newSprite->SetX(randomX);
            newSprite->SetY(randomY);
            
            // load the bitmap if a path is stored
            if (!it->second.bitmapPath.empty()) {
                newSprite->LoadBitmap(it->second.bitmapPath);
            }
            it->second.instances.emplace_back(newSprite);
            it->second.currentCount++;
            return true;
        }
        return false;
    }

    // select a sprite at given coordinates
    bool SelectSpriteAt(int x, int y) {
        // first, deselect current sprite if any
        if (selectedSprite) {
            selectedSprite->SetSelected(false);
            selectedSprite = nullptr;
        }

        // check each sprite group
        for (auto& pair : spriteGroups) {
            auto& group = pair.second;
            // check sprites in reverse order (top to bottom)
            for (auto it = group.instances.rbegin(); it != group.instances.rend(); ++it) {
                if ((*it)->ContainsPoint(x, y)) {
                    selectedSprite = it->get();
                    selectedSprite->SetSelected(true);
                    BringSelectedSpriteToFront();
                    return true;
                }
            }
        }
        return false;
    }

    // deselect current sprite
    void DeselectSprite() {
        if (selectedSprite) {
            selectedSprite->SetSelected(false);
            selectedSprite = nullptr;
        }
    }

    // bring the selected sprite to the front
    void BringSelectedSpriteToFront() {
        if (!selectedSprite) return;

        // Find the sprite in its group
        for (auto& pair : spriteGroups) {
            auto& group = pair.second;
            for (auto it = group.instances.begin(); it != group.instances.end(); ++it) {
                if (it->get() == selectedSprite) {
                    // Move this sprite to the end of the list (top of render order)
                    // Using list's splice operation to move the element
                    group.instances.splice(
                        group.instances.end(),    // Move to end of list
                        group.instances,          // From the same list
                        it                        // Move this iterator's element
                    );
                    return;
                }
            }
        }
    }

    // move selected sprite to new position
    void MoveSelectedSpriteBy(int x, int y) {
        if (selectedSprite) {
            int newX = selectedSprite->GetX() + x;
            int newY = selectedSprite->GetY() + y;
            // keep sprite within window bounds
            newX = std::max(0, std::min(newX, WINDOW_WIDTH - selectedSprite->GetWidth()));
            newY = std::max(0, std::min(newY, WINDOW_HEIGHT - selectedSprite->GetHeight()));
            
            selectedSprite->SetPosition(newX, newY);
        }
    }

    bool HasSpace(const std::string& typeName) const {
        auto it = spriteGroups.find(typeName);
        if (it == spriteGroups.end()) return false;
        return it->second.currentCount < it->second.maxInstances;
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

    bool mergeSpriteGroups(const std::string& targetGroup, const std::string& sourceGroup) {
        auto targetIt = spriteGroups.find(targetGroup);
        auto sourceIt = spriteGroups.find(sourceGroup);
        
        if (targetIt != spriteGroups.end() && sourceIt != spriteGroups.end()) {
            // Splice the source list into the target list
            targetIt->second.instances.splice(
                targetIt->second.instances.end(),
                sourceIt->second.instances
            );
            // Update counts
            targetIt->second.currentCount += sourceIt->second.currentCount;
            // Remove the now-empty source group
            spriteGroups.erase(sourceIt);
            return true;
        }
        return false;
    }
};

#endif // SPRITE_MANAGER_LIST_H
