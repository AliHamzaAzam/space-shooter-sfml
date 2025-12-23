#pragma once

#include "Entity.hpp"

class Bullet : public Entity {
public:
    Bullet(float x, float y);
    ~Bullet() override = default;
    
    void update(float dt) override;
    void draw(sf::RenderWindow& window) const override;
    
    void setDirection(float dx, float dy);
    bool isHit() const { return hit; }
    void markHit() { hit = true; destroy(); }

private:
    float speed = 15.f;
    bool hit = false;
};
