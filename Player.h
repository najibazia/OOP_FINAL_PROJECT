

#pragma once
#include "Entity.h"
#include "Projectile.h"
#include <SFML/Graphics.hpp>
#include <memory>

enum class PlayerState { IDLE, ATTACK };

class Player : public Entity
{
public:
    Player(sf::Vector2f startPos,
           sf::Vector2f towerPos,
           float mapW, float mapH);

    void update(float dt) override;
    void draw(sf::RenderWindow& window) override;

    std::unique_ptr<Projectile> shoot();

    static constexpr float ARROW_RANGE = 420.f;

    sf::Vector2f getPosition() const override;

private:
    sf::Vector2f towerPos;
    float        mapW, mapH;

    static constexpr float TOWER_BLOCK_R = 52.f;
    static constexpr float MOVE_SPEED    = 160.f;

    PlayerState state;

    bool facingRight;
    void applyFacing();

    float attackCooldown;
    float attackTimer;
    int   nextArrowID;

    sf::Texture idleTex;
    sf::Texture attackTex;
    int         frameIndex;
    float       frameTimer;
    float       frameSpeed;

    static const int FRAME_W     = 48;
    static const int FRAME_H     = 48;
    static const int FRAME_COUNT = 6;

    void updateMovement(float dt);
    void updateAnimation(float dt);
    void clampToMap();
    void blockTower();
};
