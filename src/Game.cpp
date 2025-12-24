#include "Game.hpp"
#include "entities/Entity.hpp"
#include "entities/Spaceship.hpp"
#include "entities/Bullet.hpp"
#include "entities/enemies/Enemy.hpp"
#include "entities/enemies/Bomb.hpp"
#include "entities/enemies/InvaderTypes.hpp"
#include "entities/enemies/Dragon.hpp"
#include "entities/enemies/Monster.hpp"
#include "entities/powerups/Lives.hpp"
#include "entities/powerups/PowerUp.hpp"
#include "entities/powerups/Fire.hpp"
#include "entities/powerups/Danger.hpp"
#include <iostream>
#include <filesystem>
#include <cstdlib>
#include <ctime>

Game::Game() 
    : window(sf::VideoMode({WINDOW_WIDTH, WINDOW_HEIGHT}), "Space Shooter")
    , state(GameState::Playing)
    , isRunning(true)
    , levelManager(resources)
{
    std::srand(static_cast<unsigned int>(std::time(nullptr)));
    window.setFramerateLimit(60);
    
    // Set up resource manager with path relative to executable
    auto execPath = std::filesystem::current_path();
    resources.setBasePath(execPath / "assets");
    
    // Load background
    try {
        auto& bgTexture = resources.getTexture("background_1.png");
        background.emplace(bgTexture);
        background->setScale({2.f, 1.5f});
    } catch (const std::exception& e) {
        std::cerr << "Warning: " << e.what() << std::endl;
    }
    
    // Create player
    player = std::make_unique<Spaceship>(resources);
    
    // Spawn first level
    levelManager.spawnLevel(enemies);
    
    std::cout << "Game initialized successfully!" << std::endl;
}

Game::~Game() {
    // Clear sprites BEFORE resources are destroyed
    player.reset();
    enemies.clear();
    background.reset();
    entities.clear();
    resources.clear();
    std::cout << "Game shutting down..." << std::endl;
}



void Game::run() {
    sf::Clock clock;
    float accumulator = 0.f;
    
    while (window.isOpen() && isRunning) {
        float dt = clock.restart().asSeconds();
        accumulator += dt;
        
        processEvents();
        
        // Fixed timestep update
        while (accumulator >= TIME_PER_FRAME) {
            update(TIME_PER_FRAME);
            accumulator -= TIME_PER_FRAME;
        }
        
        render();
    }
}

void Game::processEvents() {
    while (const auto event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) {
            window.close();
        }
        else if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {
            handleKeyPress(keyPressed->code);
        }
        else if (const auto* keyReleased = event->getIf<sf::Event::KeyReleased>()) {
            handleKeyRelease(keyReleased->code);
        }
    }
}

void Game::handleKeyPress(sf::Keyboard::Key key) {
    if (key == sf::Keyboard::Key::Escape) {
        if (state == GameState::Playing) {
            state = GameState::Paused;
            std::cout << "Game paused" << std::endl;
        } else if (state == GameState::Paused) {
            state = GameState::Playing;
            std::cout << "Game resumed" << std::endl;
        }
    }
    
    // Forward to player
    if (player && state == GameState::Playing) {
        player->onKeyPressed(key);
    }
}

void Game::handleKeyRelease(sf::Keyboard::Key key) {
    // Forward to player
    if (player) {
        player->onKeyReleased(key);
    }
}

void Game::update(float dt) {
    if (state != GameState::Playing) {
        return;
    }
    
    // Update player
    if (player) {
        player->update(dt);
    }
    
    // Update enemies (pass player position for aimed bombs)
    for (auto& enemy : enemies) {
        if (player) {
            enemy->setPlayerPosition(player->getPosition().x, player->getPosition().y);
        }
        enemy->update(dt);
    }
    
    // Update other entities
    for (auto& entity : entities) {
        entity->update(dt);
    }
    
    // Update power-ups
    for (auto& p : powerups) {
        p->update(dt);
    }
    
    // Spawn power-ups randomly
    powerUpSpawnTimer += dt;
    if (powerUpSpawnTimer > 5.0f) { // Every 5 seconds roughly
        if (std::rand() % 100 < 40) { // 40% chance every 5s check? or just spawn every 5s?
            spawnPowerUp();
        }
        powerUpSpawnTimer = 0.f;
    }
    
    // Check collisions
    checkCollisions();
    
    // Check level completion
    checkLevelComplete();
    
    // Cleanup destroyed entities
    cleanupDestroyedEntities();
}

void Game::spawnPowerUp() {
    float x = static_cast<float>(std::rand() % (WINDOW_WIDTH - 50));
    float y = -50.f; // Start above screen
    
    int type = std::rand() % 4;
    switch (type) {
        case 0:
            powerups.push_back(std::make_unique<Lives>(resources, x, y));
            break;
        case 1:
            powerups.push_back(std::make_unique<PowerUp>(resources, x, y));
            break;
        case 2:
            powerups.push_back(std::make_unique<Fire>(resources, x, y));
            break;
        case 3:
            powerups.push_back(std::make_unique<Danger>(resources, x, y));
            break;
    }
    std::cout << "Spawned PowerUp Type: " << type << std::endl;
}

void Game::checkCollisions() {
    if (!player) return;
    
    auto& bullets = player->getBullets();
    
    // Check each bullet against each enemy
    for (auto& bullet : bullets) {
        if (bullet->isDestroyed()) continue;
        
        for (auto& enemy : enemies) {
            if (enemy->isDestroyed() || enemy->isDead()) continue;
            
            // Check collision
            if (bullet->intersects(*enemy)) {
                // Only mark bullet as hit if player doesn't have piercing bullets
                if (!player->hasPiercingBullets()) {
                    bullet->markHit();
                }
                enemy->takeDamage(1);
                
                if (enemy->isDead()) {
                    player->addScore(enemy->getScoreValue());
                    std::cout << "Score: " << player->getScore() << std::endl;
                }
                
                // If piercing, continue hitting enemies; otherwise break
                if (!player->hasPiercingBullets()) {
                    break;  // Bullet can only hit one enemy
                }
            }
        }
    }
    
    // Check enemy bombs against player
    for (auto& enemy : enemies) {
        // Handle Dragon's spread bombs (multiple bombs)
        if (auto* dragon = dynamic_cast<Dragon*>(enemy.get())) {
            for (auto* bomb : dragon->getActiveBombs()) {
                if (bomb && !bomb->isDestroyed() && bomb->intersects(*player)) {
                    bomb->markHit();
                    player->damage(1);
                    if (player->getHealth() <= 0) {
                        state = GameState::GameOver;
                        std::cout << "GAME OVER! Final Score: " << player->getScore() << std::endl;
                    }
                }
            }
            continue;  // Dragon handles its own bombs
        }
        
        // Handle Monster's tracking bombs (multiple bombs)
        if (auto* monster = dynamic_cast<Monster*>(enemy.get())) {
            for (auto* bomb : monster->getActiveBombs()) {
                if (bomb && !bomb->isDestroyed() && bomb->intersects(*player)) {
                    bomb->markHit();
                    player->damage(1);
                    if (player->getHealth() <= 0) {
                        state = GameState::GameOver;
                        std::cout << "GAME OVER! Final Score: " << player->getScore() << std::endl;
                    }
                }
            }
            continue;  // Monster handles its own bombs
        }
        
        // Handle regular single bomb (other enemies)
        auto* bomb = enemy->getActiveBomb();
        if (bomb && !bomb->isDestroyed()) {
            if (bomb->intersects(*player)) {
                bomb->markHit();
                player->damage(1);
                if (player->getHealth() <= 0) {
                    state = GameState::GameOver;
                    std::cout << "GAME OVER! Final Score: " << player->getScore() << std::endl;
                }
            }
        }
    }
    
    // Check power-ups vs player
    for (auto& p : powerups) {
        if (!p->isDestroyed() && p->intersects(*player)) {
            p->applyEffect(*player);
            p->destroy();
        }
    }
}

void Game::cleanupDestroyedEntities() {
    // Remove destroyed enemies
    enemies.erase(
        std::remove_if(enemies.begin(), enemies.end(),
            [](const auto& e) { return e->isDestroyed(); }),
        enemies.end()
    );
    
    // Remove destroyed powerups
    powerups.erase(
        std::remove_if(powerups.begin(), powerups.end(),
            [](const auto& p) { return p->isDestroyed(); }),
        powerups.end()
    );
    
    // Remove other destroyed entities
    entities.erase(
        std::remove_if(entities.begin(), entities.end(),
            [](const auto& e) { return e->isDestroyed(); }),
        entities.end()
    );
}

void Game::checkLevelComplete() {
    if (state != GameState::Playing) return;
    
    if (levelManager.isLevelComplete(enemies)) {
        levelManager.nextLevel();
        
        if (levelManager.isVictory()) {
            std::cout << "=== CONGRATULATIONS! YOU WIN! ===" << std::endl;
            std::cout << "Final Score: " << player->getScore() << std::endl;
            // Loop back to level 1
            levelManager.nextLevel();
        }
        
        // Spawn next level enemies
        levelManager.spawnLevel(enemies);
    }
}

void Game::render() {
    window.clear(sf::Color::Black);
    
    if (background) {
        window.draw(*background);
    }
    
    // Draw enemies
    for (const auto& enemy : enemies) {
        enemy->draw(window);
    }
    
    // Draw power-ups
    for (const auto& p : powerups) {
        p->draw(window);
    }
    
    // Draw player
    if (player) {
        player->draw(window);
    }
    
    // Draw other entities
    for (const auto& entity : entities) {
        entity->draw(window);
    }
    
    window.display();
}
