#pragma once

#include "AddOn.hpp"

class PowerUp : public AddOn {
public:
    PowerUp(ResourceManager& resources, float x, float y)
        : AddOn(resources, "PNG/Power-ups/star_gold.png", x, y)
    {
    }
    
    void applyEffect(Spaceship& player) override {
        player.addScore(50);
    }
};
