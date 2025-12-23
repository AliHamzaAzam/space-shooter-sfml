#pragma once

#include "Enemy.hpp"

class Invader : public Enemy {
public:
    Invader(ResourceManager& resources, int health, char type);
    ~Invader() override = default;
    
    void onTransform() override;
    
    // Set texture rect for sprite sheet (100x100 frames)
    void setFrame(int col, int row);

protected:
    static constexpr int FRAME_SIZE = 100;
};
