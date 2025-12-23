#pragma once

#include "Entity.hpp"
#include "../ResourceManager.hpp"
#include <vector>
#include <memory>

class Bullet;

class Spaceship : public Entity {
public:
    Spaceship();
    ~Spaceship() override = default;
    
    void update(float dt) override;
    void draw(sf::RenderWindow& window) const override;
    
    void move(const std::string& direction);
    void fire();
    
    // Getters
    int getHealth() const { return health; }
    int getScore() const { return score; }
    
    // Setters
    void addScore(int points) { score += points; }
    void damage(int amount);
    void heal(int amount);

private:
    void wrapAroundScreen();
    
private:
    std::vector<std::unique_ptr<Bullet>> bullets;
    sf::Clock fireCooldown;
    
    float speed = 10.f;
    int health = 3;
    int score = 0;
    
    static constexpr float FIRE_COOLDOWN_MS = 200.f;
    static constexpr int MAX_HEALTH = 3;
};
