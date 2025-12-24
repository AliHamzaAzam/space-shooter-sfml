#pragma once

#include <SFML/Graphics.hpp>
#include <string>
#include <unordered_map>
#include <filesystem>
#include <stdexcept>
#include <iostream>
#include <memory>

class ResourceManager {
public:
    ResourceManager() {
        basePath = std::filesystem::current_path() / "assets";
        std::cout << "Resource base path: " << basePath << std::endl;
    }
    ~ResourceManager() = default;
    
    // Non-copyable
    ResourceManager(const ResourceManager&) = delete;
    ResourceManager& operator=(const ResourceManager&) = delete;

    void setBasePath(const std::filesystem::path& path) {
        basePath = path;
        std::cout << "Resource base path updated: " << basePath << std::endl;
    }

    sf::Texture& getTexture(const std::string& filename) {
        auto it = textures.find(filename);
        if (it != textures.end()) {
            return it->second;
        }

        std::filesystem::path fullPath = basePath / "images" / filename;
        sf::Texture texture;
        if (!texture.loadFromFile(fullPath.string())) {
            throw std::runtime_error("Failed to load texture: " + fullPath.string());
        }
        
        textures[filename] = std::move(texture);
        return textures[filename];
    }

    sf::Font& getFont(const std::string& filename) {
        auto it = fonts.find(filename);
        if (it != fonts.end()) {
            return it->second;
        }

        std::filesystem::path fullPath = basePath / "fonts" / filename;
        sf::Font font;
        if (!font.openFromFile(fullPath.string())) {
            throw std::runtime_error("Failed to load font: " + fullPath.string());
        }
        
        fonts[filename] = std::move(font);
        return fonts[filename];
    }
    
    std::string getBasePath() const {
        return basePath.string();
    }
    
    void clear() {
        textures.clear();
        fonts.clear();
    }

private:
    std::filesystem::path basePath;
    std::unordered_map<std::string, sf::Texture> textures;
    std::unordered_map<std::string, sf::Font> fonts;
};
