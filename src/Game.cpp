#include "Game.hpp"
#include "entities/Entity.hpp"
#include "entities/Spaceship.hpp"
#include <iostream>
#include <filesystem>

Game::Game() 
    : window(sf::VideoMode({WINDOW_WIDTH, WINDOW_HEIGHT}), "Space Shooter")
    , state(GameState::Playing)
    , isRunning(true)
{
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
    
    std::cout << "Game initialized successfully!" << std::endl;
}

Game::~Game() {
    // Clear sprites BEFORE resources are destroyed
    player.reset();
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
    
    // Update all other entities
    for (auto& entity : entities) {
        entity->update(dt);
    }
    
    // Remove destroyed entities
    entities.erase(
        std::remove_if(entities.begin(), entities.end(),
            [](const auto& e) { return e->isDestroyed(); }),
        entities.end()
    );
}

void Game::render() {
    window.clear(sf::Color::Black);
    
    if (background) {
        window.draw(*background);
    }
    
    // Draw player
    if (player) {
        player->draw(window);
    }
    
    // Draw all other entities
    for (const auto& entity : entities) {
        entity->draw(window);
    }
    
    window.display();
}
