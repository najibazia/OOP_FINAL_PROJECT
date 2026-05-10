# Tower Defense Game using C++ and SFML

## Object Oriented Programming Semester Project

### Developed By
- Najiba Zia
- Jia Ali
- Humaira Shaheen

### Instructor
Behraj Khan

### Department
Department of Computer Science

---

# Project Overview

This project is a 2D Tower Defense game developed using C++ and the SFML graphics library.  
The game demonstrates the implementation of core Object-Oriented Programming (OOP) concepts including:

- Encapsulation
- Inheritance
- Polymorphism
- Modular Design
- Abstraction

The player controls an archer character responsible for defending a central tower against continuously spawning enemy waves. Enemies follow predefined waypoint paths and attack the tower upon reaching it.

The project focuses on clean software architecture, modular class design, real-time game systems, and interactive gameplay mechanics.

---

# Features

- Real-time game loop implementation
- Player movement and animation system
- Straight arrow shooting combat system
- Enemy spawning and wave progression
- Tower health and damage system
- Grid-based map system
- Level progression and difficulty scaling
- Collision detection system
- Object-Oriented architecture

---

# Technologies Used

- C++
- SFML (Simple and Fast Multimedia Library)
- Object-Oriented Programming Principles

---

# OOP Concepts Implemented

## Encapsulation
Class data such as position, movement speed, health points, and timers are kept private and accessed through controlled public methods.

## Inheritance
The following classes inherit from a common `Entity` base class:

- Player
- Enemy
- Projectile

This reduces code duplication and improves scalability.

## Polymorphism
Virtual functions such as `update()` and `draw()` are overridden by derived classes to provide specialized behavior.

## Abstraction
Complex game systems are divided into manageable modules such as:

- Game
- Map
- Tower
- Enemy
- Projectile
- Player

---

# Project Structure

```plaintext
TowerDefenseGame/
│
├── src/                 # Source (.cpp) files
├── include/             # Header (.h) files
├── assets/              # Textures, fonts, sounds
├── build/               # Build directory
├── README.md
├── CMakeLists.txt
├── Makefile
├── TowerDefenseReport.pdf
└── game.exe
```

---

# Main Classes

## Game
Controls:
- Main game loop
- Window handling
- Event processing
- Rendering
- Game state management

## Player
Handles:
- Movement
- Animation
- Shooting mechanics

## Enemy
Handles:
- Path following
- Movement
- Damage dealing

## Projectile
Handles:
- Straight arrow movement
- Collision interactions
- Enemy damage

## Tower
Represents the defensive structure with a health system.

## Map and Tile
Manage:
- Grid-based environment
- Terrain rendering
- Path generation

## LevelConfig
Controls:
- Enemy scaling
- Spawn intervals
- Difficulty progression

---

# Game Controls

| Key | Action |
|-----|--------|
| Up Arrow | Move Up |
| Down Arrow | Move Down |
| Left Arrow | Move Left |
| Right Arrow | Move Right |
| K | Shoot |
| ESC | Exit Game |

---

# Build Instructions

## Requirements

Before building the project, ensure the following are installed:

- C++ Compiler (G++ recommended)
- SFML Library
- CMake (optional)

---

# Building Using CMake

```bash
mkdir build
cd build
cmake ..
make
```

---

# Building Using G++

```bash
g++ src/*.cpp -o game -lsfml-graphics -lsfml-window -lsfml-system
```

---

# Running the Game

## Linux / macOS

```bash
./game
```

## Windows

```bash
game.exe
```

---

# Gameplay Objective

The objective of the game is to defend the central tower from incoming enemy waves. The player must eliminate enemies using straight arrow attacks before the tower loses all health points.

Difficulty increases progressively as enemy attributes scale with levels.

---

# Testing

The following systems were tested successfully:

- Player movement in all directions
- Enemy spawning system
- Projectile collision detection
- Tower damage handling
- Level progression
- Rendering and animation updates

---

# Future Improvements

Possible future enhancements include:

- Additional enemy types
- Power-ups and upgrades
- Sound effects and background music
- Improved enemy AI and pathfinding
- Save/load game functionality
- Enhanced visual effects and shaders

---

# Conclusion

This project successfully demonstrates the practical implementation of Object-Oriented Programming concepts within a real-time interactive game environment using C++ and SFML.

The modular structure and scalable architecture allow the project to be extended with additional gameplay mechanics and graphical improvements in the future.

---

# License

This project was developed for academic purposes as part of the Object Oriented Programming Semester Project.