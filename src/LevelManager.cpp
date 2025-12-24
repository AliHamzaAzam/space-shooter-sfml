#include "LevelManager.hpp"
#include "ResourceManager.hpp"
#include "entities/enemies/Bomb.hpp"
#include <cmath>
#include <cstdlib>
#include <iostream>

LevelManager::LevelManager(ResourceManager& res) : resources(res) {}

void LevelManager::spawnLevel(std::vector<std::unique_ptr<Enemy>>& enemies) {
    enemies.clear();
    
    switch (state) {
        case LevelState::Level1:
            std::cout << "=== LEVEL 1 ===" << std::endl;
            spawnGridFormation(enemies, 3, 5);  // 15 enemies
            break;
        case LevelState::Level2:
            std::cout << "=== LEVEL 2 ===" << std::endl;
            spawnCircleFormation(enemies, 12);
            break;
        case LevelState::Level3:
            std::cout << "=== LEVEL 3 ===" << std::endl;
            spawnGridFormation(enemies, 4, 5);  // 20 enemies
            break;
        case LevelState::BossDragon:
            std::cout << "=== DRAGON BOSS ===" << std::endl;
            spawnDragon(enemies);
            break;
        case LevelState::BossMonster:
            std::cout << "=== FINAL BOSS: MONSTER ===" << std::endl;
            spawnMonster(enemies);
            break;
        case LevelState::Victory:
            std::cout << "=== VICTORY! ===" << std::endl;
            break;
    }
}

bool LevelManager::isLevelComplete(const std::vector<std::unique_ptr<Enemy>>& enemies) const {
    if (state == LevelState::Victory) return false;
    
    for (const auto& enemy : enemies) {
        if (!enemy->isDead() && !enemy->isDestroyed()) {
            return false;
        }
    }
    return !enemies.empty();
}

void LevelManager::nextLevel() {
    switch (state) {
        case LevelState::Level1:
            state = LevelState::Level2;
            break;
        case LevelState::Level2:
            state = LevelState::Level3;
            break;
        case LevelState::Level3:
            state = LevelState::BossDragon;
            break;
        case LevelState::BossDragon:
            state = LevelState::BossMonster;
            break;
        case LevelState::BossMonster:
            state = LevelState::Victory;
            break;
        case LevelState::Victory:
            // Loop back to Level 1
            state = LevelState::Level1;
            break;
    }
}

int LevelManager::getLevelNumber() const {
    switch (state) {
        case LevelState::Level1: return 1;
        case LevelState::Level2: return 2;
        case LevelState::Level3: return 3;
        case LevelState::BossDragon: return 4;
        case LevelState::BossMonster: return 5;
        case LevelState::Victory: return 0;
    }
    return 0;
}

std::unique_ptr<Enemy> LevelManager::createRandomInvader(float x, float y) {
    int type = std::rand() % 3;
    switch (type) {
        case 0: return std::make_unique<Alpha>(resources, x, y);
        case 1: return std::make_unique<Beta>(resources, x, y);
        default: return std::make_unique<Gamma>(resources, x, y);
    }
}

void LevelManager::spawnGridFormation(std::vector<std::unique_ptr<Enemy>>& enemies, int rows, int cols) {
    float spacingX = 180.f;  // Wide spacing for oscillation
    float spacingY = 140.f;  // Increased vertical spacing
    float startX = (SCREEN_WIDTH - (cols - 1) * spacingX) / 2.f;
    float startY = 50.f;
    
    for (int row = 0; row < rows; row++) {
        for (int col = 0; col < cols; col++) {
            float x = startX + col * spacingX;
            float y = startY + row * spacingY;
            enemies.push_back(createRandomInvader(x, y));
        }
    }
}

void LevelManager::spawnCircleFormation(std::vector<std::unique_ptr<Enemy>>& enemies, int count) {
    float centerX = SCREEN_WIDTH / 2.f;
    float centerY = 280.f;
    float radius = 250.f;  // Increased from 200
    
    for (int i = 0; i < count; i++) {
        float angle = (2.f * 3.14159f * i) / count;
        float x = centerX + radius * std::cos(angle);
        float y = centerY + radius * std::sin(angle);
        enemies.push_back(createRandomInvader(x, y));
    }
}

void LevelManager::spawnDragon(std::vector<std::unique_ptr<Enemy>>& enemies) {
    enemies.push_back(std::make_unique<Dragon>(resources, SCREEN_WIDTH / 2.f, 200.f));
}

void LevelManager::spawnMonster(std::vector<std::unique_ptr<Enemy>>& enemies) {
    enemies.push_back(std::make_unique<Monster>(resources, SCREEN_WIDTH / 2.f, 200.f));
}
