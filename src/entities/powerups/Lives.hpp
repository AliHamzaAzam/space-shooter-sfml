#pragma once

#include "AddOn.hpp"

class Lives : public AddOn {
public:
    Lives(ResourceManager& resources, float x, float y)
        : AddOn(resources, "PNG/Power-ups/things_silver.png", x, y)
    {
    }
    
    void applyEffect(Spaceship& player) override {
        player.addHealth(1);
    }
};
