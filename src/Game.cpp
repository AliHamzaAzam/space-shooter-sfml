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
    , state(GameState::Menu)
    , isRunning(true)
    , levelManager(resources)
    , menu(resources)
{
    std::srand(static_cast<unsigned int>(std::time(nullptr)));
    window.setFramerateLimit(60);
    
    // Load background
    try {
        auto& bgTexture = resources.getTexture("background_1.png");
        background.emplace(bgTexture);
        background->setScale({2.f, 1.5f});
    } catch (const std::exception& e) {
        std::cerr << "Warning: " << e.what() << std::endl;
    }
    
    // Load HUD font
    try {
        hudFont = resources.getFont("Lovelo-LineBold.ttf");
    } catch (const std::exception& e) {
        std::cerr << "Warning: HUD font not loaded" << std::endl;
    }
    
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
        // Handle menu states
        if (state == GameState::Menu) {
            showMainMenu();
            clock.restart();
            continue;
        }
        
        if (state == GameState::Paused) {
            auto result = menu.showPauseMenu(window);
            if (result == MenuResult::Resume) {
                state = GameState::Playing;
            } else if (result == MenuResult::Restart) {
                restartGame();
            } else if (result == MenuResult::Return || result == MenuResult::Quit) {
                state = GameState::Menu;
            }
            clock.restart();
            continue;
        }
        
        if (state == GameState::GameOver) {
            handleGameOver();
            clock.restart();
            continue;
        }
        
        float dt = clock.restart().asSeconds();
        accumulator += dt;
        
        processEvents();
        
        // Fixed timestep update
        while (accumulator >= TIME_PER_FRAME) {
            processMouseInput();  
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

void Game::processMouseInput() {
    if (!useMouseControl || !player || state != GameState::Playing) return;
    
    auto mousePos = sf::Mouse::getPosition(window);
    auto playerPos = player->getPosition();
    
    // Move ship toward mouse position
    if (mousePos.x < playerPos.x - 10) player->onKeyPressed(sf::Keyboard::Key::Left);
    else player->onKeyReleased(sf::Keyboard::Key::Left);
    
    if (mousePos.x > playerPos.x + 10) player->onKeyPressed(sf::Keyboard::Key::Right);
    else player->onKeyReleased(sf::Keyboard::Key::Right);
    
    if (mousePos.y < playerPos.y - 10) player->onKeyPressed(sf::Keyboard::Key::Up);
    else player->onKeyReleased(sf::Keyboard::Key::Up);
    
    if (mousePos.y > playerPos.y + 10) player->onKeyPressed(sf::Keyboard::Key::Down);
    else player->onKeyReleased(sf::Keyboard::Key::Down);
    
    // Click to fire
    if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)) {
        player->onKeyPressed(sf::Keyboard::Key::Space);
    } else {
        player->onKeyReleased(sf::Keyboard::Key::Space);
    }
}

void Game::update(float dt) {
    if (state != GameState::Playing) {
        return;
    }
    
    // Update player
    if (player) {
        player->update(dt);
        
        // Check for player death
        if (player->getHealth() <= 0) {
            std::cout << "GAME OVER! Final Score: " << player->getScore() << std::endl;
            state = GameState::GameOver;
            return;
        }
    }
    
    // Update explosions
    updateExplosions(dt);    
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
                    spawnExplosion(enemy->getPosition().x, enemy->getPosition().y);
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
    
    // Draw explosions
    for (const auto& exp : explosions) {
        if (exp.sprite) window.draw(*exp.sprite);
    }
    
    // Draw HUD
    renderHUD();
    
    window.display();
}

void Game::showMainMenu() {
    auto result = menu.showMainMenu(window);
    
    switch (result) {
        case MenuResult::Play:
            restartGame();
            break;
        case MenuResult::Leaderboard: {
            menu.showLeaderboard(window);
            break;
        }
        case MenuResult::Help:
            menu.showHelp(window);
            break;
        case MenuResult::About:
            menu.showAbout(window);
            break;
        case MenuResult::Options:
            selectedShipType = menu.showOptions(window, selectedShipType, useMouseControl);
            break;
        case MenuResult::Quit:
            isRunning = false;
            break;
        default:
            break;
    }
}

void Game::restartGame() {
    // Reset player with selected ship type
    player = std::make_unique<Spaceship>(resources, selectedShipType);
    
    // Reset level manager to level 1 - clear enemies directly
    enemies.clear();
    levelManager.reset();  
    levelManager.spawnLevel(enemies);
    
    // Clear power-ups
    powerups.clear();
    powerUpSpawnTimer = 0.f;
    
    state = GameState::Playing;
    std::cout << "=== NEW GAME ===" << std::endl;
}

void Game::handleGameOver() {
    std::string playerName;
    int finalScore = player ? player->getScore() : 0;
    
    auto result = menu.showGameOver(window, finalScore, playerName);
    
    if (result == MenuResult::Return || result == MenuResult::Quit) {
        state = GameState::Menu;
    }
}

void Game::renderHUD() {
    if (!player) return;
    
    // Health bar background
    sf::RectangleShape healthBg({200.f, 20.f});
    healthBg.setPosition({20.f, 20.f});
    healthBg.setFillColor(sf::Color(60, 60, 60));
    
    // Health bar (green to red based on health)
    float healthPercent = static_cast<float>(player->getHealth()) / 3.f;
    sf::RectangleShape healthBar({200.f * healthPercent, 20.f});
    healthBar.setPosition({20.f, 20.f});
    healthBar.setFillColor(healthPercent > 0.5f ? sf::Color::Green : sf::Color::Red);
    
    // Score text
    sf::Text scoreText(hudFont, "Score: " + std::to_string(player->getScore()), 24);
    scoreText.setPosition({WINDOW_WIDTH - 200.f, 20.f});
    
    // Level text
    sf::Text levelText(hudFont, "Level " + std::to_string(levelManager.getLevelNumber()), 24);
    levelText.setPosition({WINDOW_WIDTH / 2.f - 50.f, 20.f});
    
    window.draw(healthBg);
    window.draw(healthBar);
    window.draw(scoreText);
    window.draw(levelText);
}

void Game::spawnExplosion(float x, float y) {
    try {
        auto& tex = resources.getTexture("explosion.png");
        Explosion exp;
        exp.sprite.emplace(tex);
        exp.sprite->setPosition({x, y});
        exp.sprite->setScale({0.5f, 0.5f}); 
        exp.timer = Explosion::FRAME_COUNT * Explosion::FRAME_DURATION;  // Total duration
        exp.frame = 0;
        // Set first frame
        exp.sprite->setTextureRect(sf::IntRect({0, 0}, {Explosion::FRAME_SIZE, Explosion::FRAME_SIZE}));
        explosions.push_back(std::move(exp));
    } catch (...) {}
}

void Game::updateExplosions(float dt) {
    for (auto& exp : explosions) {
        exp.timer -= dt;
        
        if (exp.sprite) {
            // Calculate current frame based on elapsed time
            float elapsed = (Explosion::FRAME_COUNT * Explosion::FRAME_DURATION) - exp.timer;
            int frame = static_cast<int>(elapsed / Explosion::FRAME_DURATION);
            frame = std::min(frame, Explosion::FRAME_COUNT - 1);
            
            if (frame != exp.frame) {
                exp.frame = frame;
                exp.sprite->setTextureRect(sf::IntRect(
                    {frame * Explosion::FRAME_SIZE, 0},
                    {Explosion::FRAME_SIZE, Explosion::FRAME_SIZE}
                ));
            }
        }
    }
    // Remove finished explosions
    explosions.erase(
        std::remove_if(explosions.begin(), explosions.end(),
            [](const Explosion& e) { return e.timer <= 0; }),
        explosions.end());
}
