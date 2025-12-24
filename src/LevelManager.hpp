#pragma once

#include <vector>
#include <memory>
#include <functional>
#include "entities/enemies/Enemy.hpp"
#include "entities/enemies/InvaderTypes.hpp"
#include "entities/enemies/Dragon.hpp"
#include "entities/enemies/Monster.hpp"

class ResourceManager;

enum class LevelState {
    Level1,
    Level2,
    Level3,
    BossDragon,
    BossMonster,
    Victory
};

class LevelManager {
public:
    LevelManager(ResourceManager& resources);
    
    // Spawn enemies for current level
    void spawnLevel(std::vector<std::unique_ptr<Enemy>>& enemies);
    
    // Check if level is complete (all enemies dead)
    bool isLevelComplete(const std::vector<std::unique_ptr<Enemy>>& enemies) const;
    
    // Advance to next level
    void nextLevel();
    
    // Get current level state
    LevelState getState() const { return state; }
    int getLevelNumber() const;
    bool isVictory() const { return state == LevelState::Victory; }
    
private:
    void spawnGridFormation(std::vector<std::unique_ptr<Enemy>>& enemies, int rows, int cols);
    void spawnCircleFormation(std::vector<std::unique_ptr<Enemy>>& enemies, int count);
    void spawnDragon(std::vector<std::unique_ptr<Enemy>>& enemies);
    void spawnMonster(std::vector<std::unique_ptr<Enemy>>& enemies);
    
    std::unique_ptr<Enemy> createRandomInvader(float x, float y);
    
    ResourceManager& resources;
    LevelState state = LevelState::Level1;
    
    static constexpr float SCREEN_WIDTH = 1000.f;
    static constexpr float SCREEN_HEIGHT = 1000.f;
};
