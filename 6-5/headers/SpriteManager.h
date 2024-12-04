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
        std::string bitmapPath;
    };

    std::unordered_map<std::string, SpriteGroup> spriteGroups;

public:
    SpriteManager();
    
    bool CreateSpriteType(const std::string& typeName, int x, int y, 
                        COLORREF bgColor, int maxInstances = 10,
                        int framesPerRow = 1, bool animate = false);
                        
    bool LoadSpriteBitmap(const std::string& typeName, const std::string& bitmapPath);
    bool AddInstance(const std::string& typeName, int x = -1, int y = -1);
    void UpdateAll();
    void RenderAll(HDC hdc);
    bool HasSpace(const std::string& typeName) const;
    void MoveWalker(int dx, int dy); 
};

#endif // SPRITE_MANAGER_H
