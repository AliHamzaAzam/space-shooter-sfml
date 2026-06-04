#include "Monster.hpp"
#include "Bomb.hpp"
#include "../../ResourceManager.hpp"
#include <cmath>
#include <algorithm>

Monster::Monster(ResourceManager& resources, int x, int y)
    : Enemy(resources, 100, 'M')  // 100 HP, type 'M' for Monster
{
    const sf::Texture& tex = resources.getTexture("monster.png");
    initSprite(tex);
    
    // Scale down the large monster sprite
    sprite->setScale({0.4f, 0.4f});
    
    // Center the sprite origin
    auto bounds = sprite->getLocalBounds();
    sprite->setOrigin({bounds.size.x / 2.f, bounds.size.y / 2.f});
    
    position = {static_cast<float>(x), static_cast<float>(y)};
    sprite->setPosition(position);
    
    // Store start Y for bobbing
    startY = static_cast<float>(y);
    
    // No horizontal oscillation
    setStartPosition(static_cast<float>(x), static_cast<float>(y));
    oscillateRange = 0.f;
}

void Monster::update(float dt) {
    // Update base enemy (handles bomb timer)
    Enemy::update(dt);
    
    // Vertical bobbing movement (menacing hover)
    bobTimer += dt;
    float bobY = startY + std::sin(bobTimer * 2.f) * bobRange;
    position.y = bobY;
    
    if (sprite) {
        sprite->setPosition(position);
    }
    
    // Update all active bombs
    for (auto& b : bombs) {
        if (b && !b->isDestroyed()) {
            b->update(dt);
        }
    }
    
    // Clean up destroyed bombs
    bombs.erase(
        std::remove_if(bombs.begin(), bombs.end(),
            [](const auto& b) { return !b || b->isDestroyed(); }),
        bombs.end()
    );
    
    // Aggressive 3-way tracking attack
    bombTimer += dt;
    if (bombTimer >= getBombCooldown()) {
        bombTimer = 0.f;
        
        // Get scaled sprite size for proper positioning
        auto bounds = sprite->getGlobalBounds();
        float baseX = position.x;  // Centered on monster
        float baseY = position.y + bounds.size.y / 2.f + 10.f;  // Below monster
        float speed = getBombSpeed();
        
        // Calculate direction toward player
        float dx = playerX - baseX;
        float dy = playerY - baseY;
        float len = std::sqrt(dx * dx + dy * dy);
        if (len > 0.f) {
            dx /= len;
            dy /= len;
        } else {
            dy = 1.f;
        }
        
        // Fire 3 tracking beams with slight spread
        float spreads[] = {-0.2f, 0.f, 0.2f};
        float offsets[] = {-40.f, 0.f, 40.f};
        
        for (int i = 0; i < 3; i++) {
            auto bomb = std::make_unique<Bomb>(resources, baseX + offsets[i], baseY);
            bomb->setFallSpeed(speed);
            bomb->setDirection(dx + spreads[i], dy);
            bombs.push_back(std::move(bomb));
        }
    }
}

void Monster::draw(sf::RenderWindow& window) const {
    // Draw monster sprite
    if (sprite && !destroyed && health > 0) {
        window.draw(*sprite);
    }
    
    // Draw all tracking bombs
    for (const auto& b : bombs) {
        if (b && !b->isDestroyed()) {
            b->draw(window);
        }
    }
}

std::vector<Bomb*> Monster::getActiveBombs() {
    std::vector<Bomb*> result;
    for (auto& b : bombs) {
        if (b && !b->isDestroyed()) {
            result.push_back(b.get());
        }
    }
    return result;
}
