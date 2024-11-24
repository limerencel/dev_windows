#ifndef SPRITE_MANAGER_H
#define SPRITE_MANAGER_H

#pragma once
#include "Sprite.h"
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
    };

    std::unordered_map<std::string, SpriteGroup> spriteGroups;

public:
    bool CreateSpriteType(const std::string& typeName, int x, int y, 
                        COLORREF bgColor, int maxInstances = 10,
                        int framesPerRow = 1, bool animate = false) {
        if (spriteGroups.find(typeName) != spriteGroups.end()) {
            return false; // Type already exists
        }

        SpriteGroup group;
        group.maxInstances = maxInstances;
        group.currentCount = 1;
        
        // Create first instance
        auto sprite = std::make_unique<Sprite>(x, y, bgColor, framesPerRow, animate);
        group.instances.push_back(std::move(sprite));
        
        spriteGroups[typeName] = std::move(group);
        return true;
    }

    bool LoadSpriteBitmap(const std::string& typeName, const std::string& bitmapPath) {
        auto it = spriteGroups.find(typeName);
        if (it == spriteGroups.end() || it->second.instances.empty()) {
            return false;
        }
        return it->second.instances[0]->LoadBitmap(bitmapPath);
    }

    bool AddInstance(const std::string& typeName) {
        auto it = spriteGroups.find(typeName);
        if (it == spriteGroups.end()) return false;

        auto& group = it->second;
        if (group.currentCount >= group.maxInstances) return false;

        // Clone properties from first instance
        auto& firstSprite = group.instances[0];
        int newX = group.currentCount * (firstSprite->GetWidth() + 10); // 10 is spacing
        
        auto newSprite = std::make_unique<Sprite>(*firstSprite);
        newSprite->SetX(newX);
        
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
};

#endif // SPRITE_MANAGER_H
