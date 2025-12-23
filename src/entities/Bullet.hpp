#pragma once

#include "Entity.hpp"

class ResourceManager;

class Bullet : public Entity {
public:
    Bullet(ResourceManager& resources, float x, float y, const std::string& textureName = "PNG/Lasers/laserBlue01.png");
    ~Bullet() override = default;
    
    void update(float dt) override;
    void draw(sf::RenderWindow& window) const override;
    
    bool isHit() const { return hit; }
    void markHit() { hit = true; destroy(); }

private:
    float speed = 600.f;  // pixels per second
    bool hit = false;
    
    static constexpr float SCREEN_HEIGHT = 1000.f;
};
