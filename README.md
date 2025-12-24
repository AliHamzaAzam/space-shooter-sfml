# Space Shooter

A classic arcade-style space shooter game built with modern C++17 and SFML 3.x.

> Originally developed in 2023 as an OOP course project. Rebuilt and modernized for GitHub publication.

![Gameplay](screenshots/gameplay.png)

## Features

- **Level Progression** - 3 levels with increasing difficulty + 2 boss battles
- **Multiple Enemy Types** - Alpha, Beta, Gamma invaders with unique attack patterns
- **Boss Battles** - Dragon (spread shots) and Monster (tracking beams)
- **Power-ups** - Health, Fire boost, Power-up, and Danger pickups
- **Ship Customization** - 3 ship types to choose from
- **Controls** - Keyboard (WASD/Arrows) or Mouse mode
- **Save System** - Complete game state saved to continue later
- **Leaderboard** - Top 10 scores saved locally

## Screenshots

| Main Menu | Options | Pause |
|:-:|:-:|:-:|
| ![Main Menu](screenshots/main_menu.png) | ![Options](screenshots/options.png) | ![Pause](screenshots/pause.png) |

## Controls

| Action | Keyboard | Mouse Mode |
|--------|----------|------------|
| Move | WASD / Arrow Keys | Mouse position |
| Fire | Space | Left Click |
| Pause | Escape | Escape |

## Building

### Requirements
- C++17 compiler
- CMake 3.16+
- SFML 3.x

### Build Steps

```bash
mkdir build && cd build
cmake ..
make -j4
cd bin && ./SpaceShooter
```

## Project Structure

```
src/
├── Game.cpp/hpp          # Main game loop, state management
├── Menu.cpp/hpp          # UI screens (main, pause, options, etc.)
├── LevelManager.cpp/hpp  # Level progression & enemy spawning
├── ResourceManager.hpp   # Texture/font caching with RAII
└── entities/
    ├── Entity.hpp        # Base class for all game objects
    ├── Spaceship.cpp/hpp # Player with momentum physics
    ├── Bullet.cpp/hpp    # Projectile system
    ├── enemies/          # Enemy hierarchy (Invader → Alpha/Beta/Gamma)
    └── powerups/         # AddOn base → Lives, Fire, PowerUp, Danger
```

## Architecture

### Design Patterns
- **Entity-Component System** - Base `Entity` class with sprites, positions, collision
- **Factory Pattern** - `LevelManager` creates enemy formations
- **Resource Manager** - Singleton-like caching for textures/fonts
- **State Machine** - `GameState` enum manages game flow

### Key Features Implementation
- **Binary Save System** - Complete state serialization (player + all enemies)
- **Sprite Sheet Animation** - Frame-based animation for explosions/ships
- **Collision Detection** - AABB intersection between bullets and enemies
- **Movement Physics** - Acceleration/friction for smooth player controls

## License

[MIT License](LICENSE)
