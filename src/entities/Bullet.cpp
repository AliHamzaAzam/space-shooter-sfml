#include "Bullet.hpp"

Bullet::Bullet(float x, float y) {
    setPosition(x, y);
    setVelocity(0.f, -1.f); // Default: move upward
    // TODO: Load texture with initSprite()
}

void Bullet::update(float dt) {
    position += velocity * speed * dt * 60.f; // Scale for 60fps equivalent
    if (sprite) {
        sprite->setPosition(position);
    }
    
    // Remove if off screen
    if (position.y < -50.f || position.y > 1050.f) {
        destroy();
    }
}

void Bullet::draw(sf::RenderWindow& window) const {
    if (sprite) {
        window.draw(*sprite);
    }
}

void Bullet::setDirection(float dx, float dy) {
    velocity = {dx, dy};
}
