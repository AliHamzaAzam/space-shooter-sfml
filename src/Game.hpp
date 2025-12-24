#pragma once

#include <SFML/Graphics.hpp>
#include <memory>
#include <vector>
#include <string>
#include <optional>
#include "ResourceManager.hpp"
#include "LevelManager.hpp"
#include "Menu.hpp"
#include "entities/powerups/AddOn.hpp"

// Forward declarations
class Entity;
class Spaceship;
class Enemy;

enum class GameState {
    Menu,
    Playing,
    Paused,
    GameOver,
    Victory
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
    void processMouseInput();  
    
    // Game logic
    void spawnPowerUp();
    void checkCollisions();
    void cleanupDestroyedEntities();
    void checkLevelComplete();
    void restartGame();
    void showMainMenu();
    void handleGameOver();
    void renderHUD();
    void spawnExplosion(float x, float y);
    void updateExplosions(float dt);
    void saveGame();
    bool loadGame(); 

private:
    static constexpr unsigned int WINDOW_WIDTH = 1000;
    static constexpr unsigned int WINDOW_HEIGHT = 1000;
    static constexpr float TIME_PER_FRAME = 1.f / 60.f;
    
    // Resources must be declared BEFORE sprites that use them
    ResourceManager resources;
    LevelManager levelManager;
    Menu menu;
    
    sf::RenderWindow window;
    std::optional<sf::Sprite> background;
    
    GameState state;
    bool isRunning;
    
    // Player
    std::unique_ptr<Spaceship> player;
    
    // Enemies
    std::vector<std::unique_ptr<Enemy>> enemies;
    
    // Power-ups
    std::vector<std::unique_ptr<AddOn>> powerups;
    float powerUpSpawnTimer = 0.f;
    
    // Settings
    int selectedShipType = 1;  // 1=red, 2=green, 3=orange
    bool useMouseControl = false;  // Mouse vs keyboard
    sf::Font hudFont;
    
    // Explosions (4-frame animation, 552x138 sheet)
    struct Explosion {
        std::optional<sf::Sprite> sprite;
        float timer = 0.f;
        int frame = 0;
        static constexpr int FRAME_COUNT = 4;
        static constexpr int FRAME_SIZE = 138;  
        static constexpr float FRAME_DURATION = 0.1f;
    };
    std::vector<Explosion> explosions;
    
    // Other entities
    std::vector<std::unique_ptr<Entity>> entities;
};
