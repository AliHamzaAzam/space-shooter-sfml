#pragma once

#include "../Entity.hpp"
#include <memory>

class ResourceManager;
class Bomb;

class Enemy : public Entity {
public:
    Enemy(ResourceManager& resources, int health, char type);
    virtual ~Enemy();
    
    void update(float dt) override;
    void draw(sf::RenderWindow& window) const override;
    
    // Virtual methods for subclasses - attack patterns
    virtual void onTransform() {}
    virtual float getBombCooldown() const { return 5.f; }
    virtual float getBombSpeed() const { return 300.f; }
    virtual bool hasBombSpread() const { return false; }
    virtual bool hasAimedBombs() const { return false; }
    
    // Movement pattern - override in subclasses
    virtual float getMoveSpeed() const { return 50.f; }
    
    // Combat
    void takeDamage(int amount);
    bool isDead() const { return health <= 0; }
    
    // Bombing
    void tryDropBomb(float dt);
    Bomb* getActiveBomb() { return bomb.get(); }
    
    // Getters
    int getHealth() const { return health; }
    char getType() const { return type; }
    int getScoreValue() const;
    
    // Movement
    void moveBy(float dx, float dy);
    void setTargetPosition(float x, float y);
    void setStartPosition(float x, float y);
    
    // Set player position for aimed bombs
    void setPlayerPosition(float x, float y) { playerX = x; playerY = y; }

protected:
    ResourceManager& resources;
    std::unique_ptr<Bomb> bomb;
    
    int health;
    char type;
    float bombTimer = 0.f;
    float playerX = 500.f;
    float playerY = 850.f;
    
    // Movement oscillation
    float startX = 0.f;
    float oscillateTimer = 0.f;
    float oscillateRange = 100.f;  // How far to move left/right
    
    static constexpr float SCREEN_WIDTH = 1000.f;
    static constexpr float SCREEN_HEIGHT = 1000.f;
};
