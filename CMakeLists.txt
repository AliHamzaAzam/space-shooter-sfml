cmake_minimum_required(VERSION 3.16)
project(SpaceShooter LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Find SFML 3.x
find_package(SFML 3 COMPONENTS Graphics Window System Audio REQUIRED)

# Source files
set(SOURCES
    src/main.cpp
    src/Game.cpp
    src/ResourceManager.cpp
    src/LevelManager.cpp
    src/entities/Entity.cpp
    src/entities/Spaceship.cpp
    src/entities/Bullet.cpp
    src/entities/enemies/Bomb.cpp
    src/entities/enemies/Enemy.cpp
    src/entities/enemies/Invader.cpp
    src/entities/enemies/InvaderTypes.cpp
    src/entities/enemies/Dragon.cpp
    src/entities/enemies/Monster.cpp
)

# Headers
set(HEADERS
    src/Game.hpp
    src/ResourceManager.hpp
    src/LevelManager.hpp
    src/entities/Entity.hpp
    src/entities/Spaceship.hpp
    src/entities/Bullet.hpp
    src/entities/enemies/Bomb.hpp
    src/entities/enemies/Enemy.hpp
    src/entities/enemies/Invader.hpp
    src/entities/enemies/InvaderTypes.hpp
    src/entities/enemies/Dragon.hpp
    src/entities/enemies/Monster.hpp
)

# Create executable
add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

# Link SFML
target_link_libraries(${PROJECT_NAME} PRIVATE 
    SFML::Graphics 
    SFML::Window 
    SFML::System 
    SFML::Audio
)

# Copy assets to build directory
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/assets $<TARGET_FILE_DIR:${PROJECT_NAME}>/assets
)

# Set executable output directory
set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)
