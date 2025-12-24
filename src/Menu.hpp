#pragma once

#include <SFML/Graphics.hpp>
#include <string>
#include <vector>
#include <functional>
#include <optional>

class ResourceManager;

enum class MenuResult {
    None,
    Play,
    Continue,
    Resume,
    Restart,
    Leaderboard,
    Options,
    Help,
    About,
    Quit,
    Return
};

class Menu {
public:
    Menu(ResourceManager& resources);
    
    // Show different menu screens - returns when user makes selection
    MenuResult showMainMenu(sf::RenderWindow& window, bool hasSaveFile);
    MenuResult showPauseMenu(sf::RenderWindow& window);
    MenuResult showGameOver(sf::RenderWindow& window, int score, std::string& outPlayerName);
    MenuResult showLeaderboard(sf::RenderWindow& window);
    MenuResult showHelp(sf::RenderWindow& window);
    MenuResult showAbout(sf::RenderWindow& window);
    int showOptions(sf::RenderWindow& window, int currentShip, bool& mouseControl);  // Returns ship type 1-3
    
    // Score file management
    void saveScore(const std::string& name, int score);
    
private:
    void setupText(sf::Text& text, const std::string& str, unsigned int size, float y);
    void centerText(sf::Text& text, float y);
    bool isMouseOver(const sf::Text& text, const sf::Vector2i& mousePos) const;
    
    ResourceManager& resources;
    sf::Font font;
    std::optional<sf::Sprite> background;
    
    // Leaderboard data
    struct Player {
        std::string name;
        int score;
    };
    std::vector<Player> leaderboard;
    void loadLeaderboard();
    
    static constexpr float SCREEN_WIDTH = 1000.f;
    static constexpr float SCREEN_HEIGHT = 1000.f;
};
