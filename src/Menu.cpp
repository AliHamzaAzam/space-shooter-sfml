#include "Menu.hpp"
#include "ResourceManager.hpp"
#include <fstream>
#include <algorithm>
#include <iostream>

Menu::Menu(ResourceManager& res) : resources(res) {
    // Load font
    try {
        auto& fontRef = resources.getFont("Lovelo-LineBold.ttf");
        font = fontRef;
    } catch (const std::exception& e) {
        std::cerr << "Failed to load menu font: " << e.what() << std::endl;
    }
    
    // Load background
    try {
        auto& bgTexture = resources.getTexture("Start_Menu.png");
        background.emplace(bgTexture);
    } catch (const std::exception& e) {
        std::cerr << "Warning: Menu background not loaded" << std::endl;
    }
    
    loadLeaderboard();
}

void Menu::setupText(sf::Text& text, const std::string& str, unsigned int size, float y) {
    text = sf::Text(font, str, size);
    centerText(text, y);
}

void Menu::centerText(sf::Text& text, float y) {
    auto bounds = text.getLocalBounds();
    text.setPosition({(SCREEN_WIDTH - bounds.size.x) / 2.f, y});
}

bool Menu::isMouseOver(const sf::Text& text, const sf::Vector2i& mousePos) const {
    auto bounds = text.getGlobalBounds();
    return bounds.contains(sf::Vector2f(mousePos));
}

MenuResult Menu::showMainMenu(sf::RenderWindow& window) {
    sf::Text title(font, "Space Shooter", 70);
    sf::Text playBtn(font, "Play", 36);
    sf::Text scoreBtn(font, "Leaderboard", 36);
    sf::Text optionsBtn(font, "Options", 36);
    sf::Text helpBtn(font, "Help", 36);
    sf::Text quitBtn(font, "Quit", 36);
    
    centerText(title, 125);
    centerText(playBtn, 300);
    centerText(scoreBtn, 370);
    centerText(optionsBtn, 440);
    centerText(helpBtn, 510);
    centerText(quitBtn, 580);
    
    while (window.isOpen()) {
        while (auto event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                window.close();
                return MenuResult::Quit;
            }
            
            if (auto* mouseBtn = event->getIf<sf::Event::MouseButtonPressed>()) {
                if (mouseBtn->button == sf::Mouse::Button::Left) {
                    auto mousePos = sf::Mouse::getPosition(window);
                    if (isMouseOver(playBtn, mousePos)) return MenuResult::Play;
                    if (isMouseOver(scoreBtn, mousePos)) return MenuResult::Leaderboard;
                    if (isMouseOver(optionsBtn, mousePos)) return MenuResult::Options;
                    if (isMouseOver(helpBtn, mousePos)) return MenuResult::Help;
                    if (isMouseOver(quitBtn, mousePos)) return MenuResult::Quit;
                }
            }
        }
        
        // Hover effects
        auto mousePos = sf::Mouse::getPosition(window);
        playBtn.setFillColor(isMouseOver(playBtn, mousePos) ? sf::Color::Yellow : sf::Color::White);
        scoreBtn.setFillColor(isMouseOver(scoreBtn, mousePos) ? sf::Color::Yellow : sf::Color::White);
        optionsBtn.setFillColor(isMouseOver(optionsBtn, mousePos) ? sf::Color::Yellow : sf::Color::White);
        helpBtn.setFillColor(isMouseOver(helpBtn, mousePos) ? sf::Color::Yellow : sf::Color::White);
        quitBtn.setFillColor(isMouseOver(quitBtn, mousePos) ? sf::Color::Yellow : sf::Color::White);
        
        window.clear();
        if (background) window.draw(*background);
        window.draw(title);
        window.draw(playBtn);
        window.draw(scoreBtn);
        window.draw(optionsBtn);
        window.draw(helpBtn);
        window.draw(quitBtn);
        window.display();
    }
    return MenuResult::Quit;
}

MenuResult Menu::showPauseMenu(sf::RenderWindow& window) {
    sf::Text title(font, "Game Paused", 60);
    sf::Text resumeBtn(font, "Resume", 36);
    sf::Text restartBtn(font, "Restart", 36);
    sf::Text exitBtn(font, "Exit to Menu", 36);
    
    centerText(title, 150);
    centerText(resumeBtn, 320);
    centerText(restartBtn, 400);
    centerText(exitBtn, 480);
    
    while (window.isOpen()) {
        while (auto event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                window.close();
                return MenuResult::Quit;
            }
            
            if (auto* keyEvt = event->getIf<sf::Event::KeyPressed>()) {
                if (keyEvt->code == sf::Keyboard::Key::Escape) {
                    return MenuResult::Resume;
                }
            }
            
            if (auto* mouseBtn = event->getIf<sf::Event::MouseButtonPressed>()) {
                if (mouseBtn->button == sf::Mouse::Button::Left) {
                    auto mousePos = sf::Mouse::getPosition(window);
                    if (isMouseOver(resumeBtn, mousePos)) return MenuResult::Resume;
                    if (isMouseOver(restartBtn, mousePos)) return MenuResult::Restart;
                    if (isMouseOver(exitBtn, mousePos)) return MenuResult::Return;
                }
            }
        }
        
        auto mousePos = sf::Mouse::getPosition(window);
        resumeBtn.setFillColor(isMouseOver(resumeBtn, mousePos) ? sf::Color::Yellow : sf::Color::White);
        restartBtn.setFillColor(isMouseOver(restartBtn, mousePos) ? sf::Color::Yellow : sf::Color::White);
        exitBtn.setFillColor(isMouseOver(exitBtn, mousePos) ? sf::Color::Yellow : sf::Color::White);
        
        window.clear();
        if (background) window.draw(*background);
        window.draw(title);
        window.draw(resumeBtn);
        window.draw(restartBtn);
        window.draw(exitBtn);
        window.display();
    }
    return MenuResult::Quit;
}

MenuResult Menu::showGameOver(sf::RenderWindow& window, int score, std::string& outPlayerName) {
    sf::Text title(font, "Game Over", 60);
    sf::Text scoreText(font, "Score: " + std::to_string(score), 40);
    sf::Text prompt(font, "Enter Your Name:", 30);
    sf::Text nameText(font, "", 36);
    sf::Text returnBtn(font, "Submit", 36);
    
    std::string playerName;
    
    centerText(title, 150);
    centerText(scoreText, 250);
    centerText(prompt, 380);
    centerText(nameText, 440);
    centerText(returnBtn, 550);
    
    while (window.isOpen()) {
        while (auto event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                window.close();
                return MenuResult::Quit;
            }
            
            if (auto* textEvt = event->getIf<sf::Event::TextEntered>()) {
                if (textEvt->unicode < 128 && textEvt->unicode != 8 && textEvt->unicode != 13) {
                    if (playerName.length() < 15) {
                        playerName += static_cast<char>(textEvt->unicode);
                        nameText.setString(playerName);
                        centerText(nameText, 440);
                    }
                } else if (textEvt->unicode == 8 && !playerName.empty()) { // Backspace
                    playerName.pop_back();
                    nameText.setString(playerName);
                    centerText(nameText, 440);
                }
            }
            
            if (auto* keyEvt = event->getIf<sf::Event::KeyPressed>()) {
                if (keyEvt->code == sf::Keyboard::Key::Enter && !playerName.empty()) {
                    outPlayerName = playerName;
                    saveScore(playerName, score);
                    return MenuResult::Return;
                }
            }
            
            if (auto* mouseBtn = event->getIf<sf::Event::MouseButtonPressed>()) {
                if (mouseBtn->button == sf::Mouse::Button::Left) {
                    auto mousePos = sf::Mouse::getPosition(window);
                    if (isMouseOver(returnBtn, mousePos) && !playerName.empty()) {
                        outPlayerName = playerName;
                        saveScore(playerName, score);
                        return MenuResult::Return;
                    }
                }
            }
        }
        
        auto mousePos = sf::Mouse::getPosition(window);
        returnBtn.setFillColor(isMouseOver(returnBtn, mousePos) ? sf::Color::Yellow : sf::Color::White);
        
        window.clear();
        if (background) window.draw(*background);
        window.draw(title);
        window.draw(scoreText);
        window.draw(prompt);
        window.draw(nameText);
        window.draw(returnBtn);
        window.display();
    }
    return MenuResult::Quit;
}

MenuResult Menu::showLeaderboard(sf::RenderWindow& window) {
    loadLeaderboard();
    
    sf::Text title(font, "Leaderboard", 60);
    sf::Text returnBtn(font, "Return", 36);
    
    centerText(title, 80);
    centerText(returnBtn, 850);
    
    std::vector<sf::Text> entries;
    for (size_t i = 0; i < leaderboard.size() && i < 10; i++) {
        std::string rank = std::to_string(i + 1) + ". " + leaderboard[i].name + " - " + std::to_string(leaderboard[i].score);
        sf::Text entry(font, rank, 28);
        centerText(entry, 180 + i * 60);
        entries.push_back(entry);
    }
    
    while (window.isOpen()) {
        while (auto event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                window.close();
                return MenuResult::Quit;
            }
            
            if (auto* mouseBtn = event->getIf<sf::Event::MouseButtonPressed>()) {
                if (mouseBtn->button == sf::Mouse::Button::Left) {
                    auto mousePos = sf::Mouse::getPosition(window);
                    if (isMouseOver(returnBtn, mousePos)) return MenuResult::Return;
                }
            }
        }
        
        auto mousePos = sf::Mouse::getPosition(window);
        returnBtn.setFillColor(isMouseOver(returnBtn, mousePos) ? sf::Color::Yellow : sf::Color::White);
        
        window.clear();
        if (background) window.draw(*background);
        window.draw(title);
        for (auto& entry : entries) {
            window.draw(entry);
        }
        window.draw(returnBtn);
        window.display();
    }
    return MenuResult::Quit;
}

MenuResult Menu::showHelp(sf::RenderWindow& window) {
    sf::Text title(font, "Help", 60);
    sf::Text controls(font, "Controls:", 36);
    sf::Text wasd(font, "WASD / Arrows - Move", 28);
    sf::Text space(font, "SPACE - Shoot", 28);
    sf::Text esc(font, "ESC - Pause", 28);
    sf::Text returnBtn(font, "Return", 36);
    
    centerText(title, 100);
    centerText(controls, 220);
    centerText(wasd, 300);
    centerText(space, 360);
    centerText(esc, 420);
    centerText(returnBtn, 600);
    
    while (window.isOpen()) {
        while (auto event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                window.close();
                return MenuResult::Quit;
            }
            
            if (auto* mouseBtn = event->getIf<sf::Event::MouseButtonPressed>()) {
                if (mouseBtn->button == sf::Mouse::Button::Left) {
                    auto mousePos = sf::Mouse::getPosition(window);
                    if (isMouseOver(returnBtn, mousePos)) return MenuResult::Return;
                }
            }
        }
        
        auto mousePos = sf::Mouse::getPosition(window);
        returnBtn.setFillColor(isMouseOver(returnBtn, mousePos) ? sf::Color::Yellow : sf::Color::White);
        
        window.clear();
        if (background) window.draw(*background);
        window.draw(title);
        window.draw(controls);
        window.draw(wasd);
        window.draw(space);
        window.draw(esc);
        window.draw(returnBtn);
        window.display();
    }
    return MenuResult::Quit;
}

void Menu::loadLeaderboard() {
    leaderboard.clear();
    std::ifstream file(resources.getBasePath() + "/scores.txt");
    if (file.is_open()) {
        std::string name;
        int score;
        while (file >> name >> score) {
            leaderboard.push_back({name, score});
        }
        file.close();
    }
    // Sort by score descending
    std::sort(leaderboard.begin(), leaderboard.end(), 
        [](const Player& a, const Player& b) { return a.score > b.score; });
}

void Menu::saveScore(const std::string& name, int score) {
    leaderboard.push_back({name, score});
    std::sort(leaderboard.begin(), leaderboard.end(),
        [](const Player& a, const Player& b) { return a.score > b.score; });
    
    // Keep top 10
    if (leaderboard.size() > 10) {
        leaderboard.resize(10);
    }
    
    // Write to file
    std::ofstream file(resources.getBasePath() + "/scores.txt");
    if (file.is_open()) {
        for (const auto& player : leaderboard) {
            file << player.name << " " << player.score << "\n";
        }
        file.close();
    }
}

int Menu::showOptions(sf::RenderWindow& window, int currentShip) {
    sf::Text title(font, "Options", 60);
    sf::Text shipLabel(font, "Select Ship:", 36);
    sf::Text returnBtn(font, "Return", 36);
    
    centerText(title, 100);
    centerText(shipLabel, 220);
    centerText(returnBtn, 700);
    
    int selectedShip = currentShip;
    
    // Load ship icons - center them evenly across screen
    std::optional<sf::Sprite> ship1, ship2, ship3;
    float shipY = 320.f;
    float shipSpacing = 250.f;
    float startX = (SCREEN_WIDTH - 2 * shipSpacing) / 2.f;
    
    try {
        auto& tex1 = resources.getTexture("IconShip1_red.png");
        ship1.emplace(tex1);
        auto bounds = ship1->getGlobalBounds();
        ship1->setPosition({startX - bounds.size.x/2.f, shipY});
    } catch (...) {}
    
    try {
        auto& tex2 = resources.getTexture("IconShip2_green.png");
        ship2.emplace(tex2);
        auto bounds = ship2->getGlobalBounds();
        ship2->setPosition({startX + shipSpacing - bounds.size.x/2.f, shipY});
    } catch (...) {}
    
    try {
        auto& tex3 = resources.getTexture("IconShip3_orange.png");
        ship3.emplace(tex3);
        auto bounds = ship3->getGlobalBounds();
        ship3->setPosition({startX + 2*shipSpacing - bounds.size.x/2.f, shipY});
    } catch (...) {}
    
    // Selection indicator - will be sized dynamically
    sf::RectangleShape selector;
    selector.setFillColor(sf::Color::Transparent);
    selector.setOutlineColor(sf::Color::Yellow);
    selector.setOutlineThickness(3.f);
    
    while (window.isOpen()) {
        while (auto event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                window.close();
                return selectedShip;
            }
            
            if (auto* mouseBtn = event->getIf<sf::Event::MouseButtonPressed>()) {
                if (mouseBtn->button == sf::Mouse::Button::Left) {
                    auto mousePos = sf::Mouse::getPosition(window);
                    
                    if (ship1 && ship1->getGlobalBounds().contains(sf::Vector2f(mousePos))) {
                        selectedShip = 1;
                    }
                    if (ship2 && ship2->getGlobalBounds().contains(sf::Vector2f(mousePos))) {
                        selectedShip = 2;
                    }
                    if (ship3 && ship3->getGlobalBounds().contains(sf::Vector2f(mousePos))) {
                        selectedShip = 3;
                    }
                    
                    if (isMouseOver(returnBtn, mousePos)) {
                        return selectedShip;
                    }
                }
            }
        }
        
        // Update selector position and size based on selected ship
        sf::FloatRect bounds;
        if (selectedShip == 1 && ship1) bounds = ship1->getGlobalBounds();
        else if (selectedShip == 2 && ship2) bounds = ship2->getGlobalBounds();
        else if (selectedShip == 3 && ship3) bounds = ship3->getGlobalBounds();
        
        selector.setSize({bounds.size.x + 10.f, bounds.size.y + 10.f});
        selector.setPosition({bounds.position.x - 5.f, bounds.position.y - 5.f});
        
        auto mousePos = sf::Mouse::getPosition(window);
        returnBtn.setFillColor(isMouseOver(returnBtn, mousePos) ? sf::Color::Yellow : sf::Color::White);
        
        window.clear();
        if (background) window.draw(*background);
        window.draw(title);
        window.draw(shipLabel);
        if (ship1) window.draw(*ship1);
        if (ship2) window.draw(*ship2);
        if (ship3) window.draw(*ship3);
        window.draw(selector);
        window.draw(returnBtn);
        window.display();
    }
    return selectedShip;
}
