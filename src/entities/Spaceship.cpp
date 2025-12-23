#include "Spaceship.hpp"
#include "Bullet.hpp"

Spaceship::Spaceship() {
    // TODO: Will be implemented later
    setPosition(340.f, 800.f);
}

void Spaceship::update(float dt) {
    // TODO: Update bullets, handle input
}

void Spaceship::draw(sf::RenderWindow& window) const {
    if (sprite) {
        window.draw(*sprite);
    }
    // TODO: Draw bullets
}

void Spaceship::move(const std::string& direction) {
    // TODO: Implement 8-directional movement
}

void Spaceship::fire() {
    // TODO: Implement firing
}

void Spaceship::wrapAroundScreen() {
    // TODO: Screen wrapping
}

void Spaceship::damage(int amount) {
    health -= amount;
    if (health < 0) health = 0;
}

void Spaceship::heal(int amount) {
    health += amount;
    if (health > MAX_HEALTH) health = MAX_HEALTH;
}
