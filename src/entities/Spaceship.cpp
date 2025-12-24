#include "Spaceship.hpp"
#include "Bullet.hpp"
#include "../ResourceManager.hpp"
#include <iostream>

Spaceship::Spaceship(ResourceManager& res, int shipType) : resources(res) {
    // Ship textures 
    std::string textureName;
    switch (shipType) {
        case 1: textureName = "playerShip1_red.png"; break;
        case 2: textureName = "playerShip2_green.png"; break;
        case 3: textureName = "playerShip3_orange.png"; break;
        default: textureName = "playerShip1_red.png"; break;
    }
    
    // Load texture and create sprite
    try {
        auto& texture = resources.getTexture(textureName);
        initSprite(texture);
        auto texSize = texture.getSize();
        if (texSize.x > 150) {  // Sprite sheet - use first frame
            sprite->setTextureRect(sf::IntRect({0, 0}, {131, 131}));
        }
        sprite->setScale({0.75f, 0.75f});
    } catch (const std::exception& e) {
        std::cerr << "Failed to load spaceship texture: " << e.what() << std::endl;
    }
    
    // Start position (center bottom)
    setPosition(SCREEN_WIDTH / 2.f - 50.f, SCREEN_HEIGHT - 150.f);
}

void Spaceship::onKeyPressed(sf::Keyboard::Key key) {
    pressedKeys.insert(key);
}

void Spaceship::onKeyReleased(sf::Keyboard::Key key) {
    pressedKeys.erase(key);
}

bool isKeyActive(const std::unordered_set<sf::Keyboard::Key>& keys, sf::Keyboard::Key key) {
    return keys.find(key) != keys.end();
}

void Spaceship::update(float dt) {
    if (health <= 0) return;
    
    // Handle fire power-up timer
    if (piercingBullets && firePowerUpTimer > 0.f) {
        firePowerUpTimer -= dt;
        if (firePowerUpTimer <= 0.f) {
            piercingBullets = false;
            std::cout << "Fire Power-Up expired!" << std::endl;
        }
    }
    
    float dx = 0.f, dy = 0.f;
    
    // Check pressed keys for movement
    if (isKeyActive(pressedKeys, sf::Keyboard::Key::Left) || 
        isKeyActive(pressedKeys, sf::Keyboard::Key::A)) {
        dx = -1.f;
    }
    if (isKeyActive(pressedKeys, sf::Keyboard::Key::Right) ||
        isKeyActive(pressedKeys, sf::Keyboard::Key::D)) {
        dx = 1.f;
    }
    if (isKeyActive(pressedKeys, sf::Keyboard::Key::Up) ||
        isKeyActive(pressedKeys, sf::Keyboard::Key::W)) {
        dy = -1.f;
    }
    if (isKeyActive(pressedKeys, sf::Keyboard::Key::Down) ||
        isKeyActive(pressedKeys, sf::Keyboard::Key::S)) {
        dy = 1.f;
    }
    
    // Normalize diagonal movement
    if (dx != 0.f && dy != 0.f) {
        dx *= 0.707f;
        dy *= 0.707f;
    }
    
    // Store input direction for animation
    float inputDx = dx, inputDy = dy;
    
    // Apply acceleration towards input direction
    if (dx != 0.f || dy != 0.f) {
        // Accelerate towards input direction
        velocity.x += dx * acceleration * dt;
        velocity.y += dy * acceleration * dt;
    } else {
        // Apply friction when no input (decelerate)
        float currentSpeed = std::sqrt(velocity.x * velocity.x + velocity.y * velocity.y);
        if (currentSpeed > 0.f) {
            float frictionAmount = friction * dt;
            if (frictionAmount >= currentSpeed) {
                velocity = {0.f, 0.f};
            } else {
                float scale = (currentSpeed - frictionAmount) / currentSpeed;
                velocity.x *= scale;
                velocity.y *= scale;
            }
        }
    }
    
    // Clamp to max speed
    float currentSpeed = std::sqrt(velocity.x * velocity.x + velocity.y * velocity.y);
    if (currentSpeed > maxSpeed) {
        float scale = maxSpeed / currentSpeed;
        velocity.x *= scale;
        velocity.y *= scale;
    }
    
    // Animate sprite based on input direction (131x131 pixel frames)
    if (sprite) {
        const int FRAME = 131;
        if (inputDx < 0 && inputDy < 0) {
            sprite->setTextureRect(sf::IntRect({0, 2 * FRAME}, {FRAME, FRAME}));
        } else if (inputDx > 0 && inputDy < 0) {
            sprite->setTextureRect(sf::IntRect({2 * FRAME, 4 * FRAME}, {FRAME, FRAME}));
        } else if (inputDx < 0 && inputDy > 0) {
            sprite->setTextureRect(sf::IntRect({FRAME, FRAME}, {FRAME, FRAME}));
        } else if (inputDx > 0 && inputDy > 0) {
            sprite->setTextureRect(sf::IntRect({2 * FRAME, 0}, {FRAME, FRAME}));
        } else if (inputDx < 0) {
            sprite->setTextureRect(sf::IntRect({3 * FRAME, 2 * FRAME}, {FRAME, FRAME}));
        } else if (inputDx > 0) {
            sprite->setTextureRect(sf::IntRect({3 * FRAME, FRAME}, {FRAME, FRAME}));
        } else if (inputDy < 0) {
            sprite->setTextureRect(sf::IntRect({0, 0}, {FRAME, FRAME}));
        } else if (inputDy > 0) {
            sprite->setTextureRect(sf::IntRect({4 * FRAME, 0}, {FRAME, FRAME}));
        }
        // Keep current frame if no input (momentum continues)
    }
    
    // Fire with space
    if (isKeyActive(pressedKeys, sf::Keyboard::Key::Space)) {
        fire();
    }
    
    // Move based on velocity (already scaled, no speed multiplier needed)
    position += velocity * dt;
    
    // Update sprite position
    if (sprite) {
        sprite->setPosition(position);
    }
    
    // Wrap around screen
    wrapAroundScreen();
    
    // Update bullets
    updateBullets(dt);
}

void Spaceship::fire() {
    if (fireCooldown.getElapsedTime().asMilliseconds() >= FIRE_COOLDOWN_MS && health > 0) {
        if (piercingBullets) {
            // Fire single centered fire bullet
            auto bullet = std::make_unique<Bullet>(resources, position.x - 15.f, position.y - 180.f, "fire.png");
            bullets.push_back(std::move(bullet));
        } else {
            // Create two bullets (left and right of ship)
            auto bullet1 = std::make_unique<Bullet>(resources, position.x + 30.f, position.y - 10.f);
            auto bullet2 = std::make_unique<Bullet>(resources, position.x + 62.f, position.y - 10.f);
            bullets.push_back(std::move(bullet1));
            bullets.push_back(std::move(bullet2));
        }
        
        fireCooldown.restart();
    }
}

void Spaceship::updateBullets(float dt) {
    for (auto& bullet : bullets) {
        bullet->update(dt);
    }
    
    bullets.erase(
        std::remove_if(bullets.begin(), bullets.end(),
            [](const auto& b) { return b->isDestroyed(); }),
        bullets.end()
    );
}

void Spaceship::wrapAroundScreen() {
    float width = sprite ? sprite->getGlobalBounds().size.x : 75.f;
    float height = sprite ? sprite->getGlobalBounds().size.y : 75.f;
    
    if (position.x < -width) {
        position.x = SCREEN_WIDTH;
    } else if (position.x > SCREEN_WIDTH) {
        position.x = -width;
    }
    
    if (position.y < -height) {
        position.y = SCREEN_HEIGHT;
    } else if (position.y > SCREEN_HEIGHT) {
        position.y = -height;
    }
    
    if (sprite) {
        sprite->setPosition(position);
    }
}

void Spaceship::draw(sf::RenderWindow& window) const {
    if (sprite && health > 0) {
        window.draw(*sprite);
    }
    
    for (const auto& bullet : bullets) {
        bullet->draw(window);
    }
}

void Spaceship::damage(int amount) {
    health -= amount;
    if (health < 0) health = 0;
    std::cout << "Health: " << health << std::endl;
}

void Spaceship::addHealth(int amount) {
    health += amount;
    // Allow going above MAX_HEALTH for powerups
    if (health > 10) health = 10;
    std::cout << "Health Up! HP: " << health << std::endl;
}

void Spaceship::activateFirePowerUp() {
    piercingBullets = true;
    firePowerUpTimer = 5.0f;  // 5 second duration
    std::cout << "Fire Power-Up! Piercing bullets for 5 seconds!" << std::endl;
}

void Spaceship::addScore(int points) { 
    score += points; 
    std::cout << "Score Up! +" << points << " Total: " << score << std::endl;
}

Spaceship::~Spaceship() {
    bullets.clear();
}
