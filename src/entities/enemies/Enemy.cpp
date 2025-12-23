#include "Enemy.hpp"
#include "Bomb.hpp"
#include "../../ResourceManager.hpp"
#include <iostream>
#include <cmath>
#include <cstdlib>

Enemy::Enemy(ResourceManager& res, int hp, char t) 
    : resources(res), health(hp), type(t) {
}

Enemy::~Enemy() {
    bomb.reset();
}

void Enemy::update(float dt) {
    if (destroyed || health <= 0) return;
    
    // Oscillating movement (sine wave)
    oscillateTimer += dt * getMoveSpeed() * 0.02f;
    float offsetX = std::sin(oscillateTimer) * oscillateRange;
    position.x = startX + offsetX;
    
    // Update sprite position
    if (sprite) {
        sprite->setPosition(position);
    }
    
    // Update bomb timer and try to drop
    bombTimer += dt;
    tryDropBomb(dt);
    
    if (bomb) {
        bomb->update(dt);
        if (bomb->isDestroyed()) {
            bomb.reset();
        }
    }
}

void Enemy::draw(sf::RenderWindow& window) const {
    if (sprite && !destroyed && health > 0) {
        window.draw(*sprite);
    }
    
    if (bomb && !bomb->isDestroyed()) {
        bomb->draw(window);
    }
}

void Enemy::takeDamage(int amount) {
    health -= amount;
    if (health <= 0) {
        destroy();
        std::cout << "Enemy " << type << " destroyed!" << std::endl;
    }
}

void Enemy::tryDropBomb(float dt) {
    if (bomb && !bomb->isDestroyed()) {
        return;  // Already have active bomb
    }
    
    if (bombTimer >= getBombCooldown()) {
        float bombX = position.x + 40.f;
        float bombY = position.y + 50.f;
        
        // Create bomb
        bomb = std::make_unique<Bomb>(resources, bombX, bombY);
        bomb->setFallSpeed(getBombSpeed());
        
        // Apply varied attack patterns
        if (hasAimedBombs()) {
            // Gamma: Aim at player position
            float dx = playerX - bombX;
            float dy = playerY - bombY;
            float length = std::sqrt(dx * dx + dy * dy);
            if (length > 0) {
                bomb->setDirection(dx / length, dy / length);
            }
        } else if (hasBombSpread()) {
            // Beta: Random spread angle (-30 to +30 degrees)
            float angle = ((rand() % 61) - 30) * 3.14159f / 180.f;
            bomb->setDirection(std::sin(angle), std::cos(angle));
        }
        // Alpha: default straight down (no change needed)
        
        bombTimer = 0.f;
    }
}

int Enemy::getScoreValue() const {
    switch (type) {
        case 'A': return 10;   // Alpha
        case 'B': return 20;   // Beta
        case 'G': return 30;   // Gamma
        case 'D': return 100;  // Dragon
        case 'M': return 100;  // Monster
        default: return 10;
    }
}

void Enemy::moveBy(float dx, float dy) {
    position.x += dx;
    position.y += dy;
    if (sprite) {
        sprite->setPosition(position);
    }
}

void Enemy::setTargetPosition(float x, float y) {
    setPosition(x, y);
}

void Enemy::setStartPosition(float x, float y) {
    startX = x;
    position.x = x;
    position.y = y;
    if (sprite) {
        sprite->setPosition(position);
    }
}
