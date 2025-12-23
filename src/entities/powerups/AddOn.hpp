#pragma once

#include "../Entity.hpp"
#include "../Spaceship.hpp"
#include "../../ResourceManager.hpp"

class AddOn : public Entity {
public:
    AddOn(ResourceManager& resources, const std::string& textureName, float x, float y)
        : resources(resources)
    {
        const sf::Texture& tex = resources.getTexture(textureName);
        initSprite(tex);
        
        position = {x, y};
        if (sprite) {
            sprite->setPosition(position);
        }
    }
    
    virtual ~AddOn() = default;
    
    void update(float dt) override {
        if (destroyed) return;
        
        // Fall downwards
        position.y += fallSpeed * dt;
        
        if (sprite) {
            sprite->setPosition(position);
        }
        
        // Destroy if off screen
        if (position.y > 1000.f) {  // Assuming 1000 height
            destroy();
        }
    }
    
    void draw(sf::RenderWindow& window) const override {
        if (sprite && !destroyed) {
            window.draw(*sprite);
        }
    }
    
    // Virtual effect to be implemented by subclasses
    virtual void applyEffect(Spaceship& player) = 0;
    
protected:
    ResourceManager& resources;
    float fallSpeed = 150.f;  // Default fall speed
};
