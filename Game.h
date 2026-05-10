#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <memory>
#include "Map.h"
#include "Enemy.h"
#include "Tower.h"
#include "Projectile.h"
#include "Levelconfig.h"
#include "EventQueue.h"
#include "Player.h"

enum class GameState { MENU, PLAYING, LEVEL_UP, GAME_OVER, VICTORY };

class Game
{
public:
    Game();
    void run();

private:
    sf::RenderWindow window;
    sf::Clock        clock;

    int       currentLevel = 1;
    GameState state        = GameState::MENU;
    float     stateTimer   = 0.f;

    std::unique_ptr<Map>    map;
    std::unique_ptr<Tower>  tower;
    std::unique_ptr<Map>    menuMap;
    std::unique_ptr<Player> player;

    std::vector<std::unique_ptr<Enemy>>      enemies;
    std::vector<std::unique_ptr<Projectile>> projectiles;

    EventQueue<SpawnEvent> spawnQueue;
    float gameTime   = 0.f;
    float spawnTimer = 0.f;

    int   killCount      = 0;
    float levelTimer     = 0.f;
    float levelTimeLimit = 60.f;

    std::string gameOverReason;

    sf::Font font;
    bool     fontLoaded = false;

    sf::RectangleShape yesButton;
    sf::RectangleShape noButton;

    void loadLevel(int level);
    void buildSpawnQueue(int level);

    void handleEvents();
    void handleMenuClick(sf::Vector2f mousePos);

    void update(float dt);
    void render();

    void updatePlaying(float dt);
    void spawnEnemy();
    void removeDeadEnemies();
    void applyEnemyAttacks(float dt);
    void updateProjectiles(float dt);
    void collectProjectiles();
    void playerShoot();

    void drawMenu();
    void drawHUD();
    void drawLevelUp();
    void drawGameOver();
    void drawVictory();

    void drawText(const std::string& str, float x, float y,
                  unsigned int size, sf::Color color);
    void drawCentredText(const std::string& str, float y,
                         unsigned int size, sf::Color color);

    const LevelConfig& getConfig() const;
};