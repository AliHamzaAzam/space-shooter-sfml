#pragma once

#include "AddOn.hpp"

class Fire : public AddOn {
public:
    Fire(ResourceManager& resources, float x, float y)
        : AddOn(resources, "PNG/Power-ups/powerupGreen_bolt.png", x, y)
    {
    }
    
    void applyEffect(Spaceship& player) override {
        player.activateFirePowerUp();
    }
};
