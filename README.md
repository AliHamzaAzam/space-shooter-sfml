# Space Shooter

A 2D space shooter game built with **SFML 3.x** and modern C++17.

## Features (Planned)
- Player spaceship with 8-directional movement
- Multiple enemy types with formations
- Power-ups and collectibles
- Boss battles
- Score tracking and leaderboard

## Building

### Requirements
- CMake 3.16+
- SFML 3.0+
- C++17 compatible compiler

### Build Instructions
```bash
mkdir build && cd build
cmake ..
cmake --build .
```

### Run
```bash
./bin/SpaceShooter
```

## Project Structure
```
new_project/
├── assets/           # Game resources
│   ├── images/       # Sprites and textures
│   ├── fonts/        # Fonts
│   └── sounds/       # Audio files
├── src/              # Source code
│   ├── entities/     # Game entities
│   ├── powerups/     # Power-up classes
│   └── ui/           # Menu and UI
└── CMakeLists.txt
```

## Controls
- **Arrow Keys**: Move spaceship
- **Space**: Fire
- **Escape**: Pause

## License
MIT
