#pragma once

#include "Entity.hpp"
#include <vector>
#include <memory>
#include <string>
#include <unordered_set>

class Bullet;
class ResourceManager;

class Spaceship : public Entity {
public:
    Spaceship(ResourceManager& resources, int shipType = 1);
    ~Spaceship() override;
    
    void update(float dt) override;
    void draw(sf::RenderWindow& window) const override;
    
    // Input handling via events
    void onKeyPressed(sf::Keyboard::Key key);
    void onKeyReleased(sf::Keyboard::Key key);
    
    // Getters
    int getHealth() const { return health; }
    int getScore() const { return score; }
    std::vector<std::unique_ptr<Bullet>>& getBullets() { return bullets; }
    
    // Setters
    void addScore(int points);
    void damage(int amount);
    void addHealth(int amount);
    void activateFirePowerUp();  // Piercing bullets + fire rate boost
    bool hasPiercingBullets() const { return piercingBullets; }
    void setScore(int s) { score = s; }
    void setHealth(int h) { health = std::min(h, MAX_HEALTH); }

private:
    void wrapAroundScreen();
    void updateBullets(float dt);
    void fire();
    
private:
    ResourceManager& resources;
    std::vector<std::unique_ptr<Bullet>> bullets;
    sf::Clock fireCooldown;
    
    // Track pressed keys
    std::unordered_set<sf::Keyboard::Key> pressedKeys;
    
    float maxSpeed = 400.f;      // max pixels per second
    float acceleration = 800.f;  // acceleration rate
    float friction = 600.f;      // deceleration when no input
    int health = 3;
    int score = 0;
    bool piercingBullets = false;
    float firePowerUpTimer = 0.f;
    
    static constexpr float FIRE_COOLDOWN_MS = 200.f;
    static constexpr int MAX_HEALTH = 3;
    static constexpr float SCREEN_WIDTH = 1000.f;
    static constexpr float SCREEN_HEIGHT = 1000.f;
};
