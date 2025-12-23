#pragma once

#include <SFML/Graphics.hpp>
#include <memory>
#include <vector>
#include <string>
#include <optional>
#include "ResourceManager.hpp"

// Forward declarations
class Entity;

enum class GameState {
    Menu,
    Playing,
    Paused,
    GameOver
};

class Game {
public:
    Game();
    ~Game();

    void run();
    
    // Access to resources for entities
    ResourceManager& getResources() { return resources; }

private:
    void processEvents();
    void update(float dt);
    void render();
    
    void handleKeyPress(sf::Keyboard::Key key);
    void handleKeyRelease(sf::Keyboard::Key key);

private:
    static constexpr unsigned int WINDOW_WIDTH = 1000;
    static constexpr unsigned int WINDOW_HEIGHT = 1000;
    static constexpr float TIME_PER_FRAME = 1.f / 60.f;
    
    // Resources must be declared BEFORE sprites that use them
    ResourceManager resources;
    
    sf::RenderWindow window;
    std::optional<sf::Sprite> background;
    
    GameState state;
    bool isRunning;
    
    // Entities
    std::vector<std::unique_ptr<Entity>> entities;
};
