#include "InvaderTypes.hpp"
#include "../../ResourceManager.hpp"

// Alpha - Health 3, Row 0
Alpha::Alpha(ResourceManager& resources, float x, float y) 
    : Invader(resources, 3, 'A') {
    setStartPosition(x, y);  // Sets both position and oscillation center
    setFrame(0, 0);
    onTransform();
}

// Beta - Health 5, Row 1
Beta::Beta(ResourceManager& resources, float x, float y) 
    : Invader(resources, 5, 'B') {
    setStartPosition(x, y);
    setFrame(0, 1);
    onTransform();
}

// Gamma - Health 7, Row 2
Gamma::Gamma(ResourceManager& resources, float x, float y) 
    : Invader(resources, 7, 'G') {
    setStartPosition(x, y);
    setFrame(0, 2);
    onTransform();
}
