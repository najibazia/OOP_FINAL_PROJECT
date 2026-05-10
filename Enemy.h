


// #pragma once
// // ══════════════════════════════════════════════════════════════════
// //  Enemy.h
// //  Concepts:
// //    • Inheritance & Polymorphism — extends Entity, overrides update/draw
// //    • Encapsulation              — private state, public interface
// //    • Wave spawning algorithm    — handled via Game's spawnTimer
// // ══════════════════════════════════════════════════════════════════
// #include "Entity.h"
// #include <vector>
// #include <memory>
// #include <SFML/Graphics.hpp>

// enum class AnimState { WALK, ATTACK, DEATH };

// class Enemy : public Entity
// {
// public:
//     Enemy(const std::vector<sf::Vector2f>& path,
//           float hp, float speed, float attackDamage);

//     void update(float dt) override;
//     void draw(sf::RenderWindow& window) override;

//     void takeDamage(float amount);

//     bool         isDead()          const;
//     bool         hasReachedEnd()   const;
//     bool         isAttacking()     const;
//     bool         isDeathDone()     const;
//     float        getHP()           const;
//     float        getMaxHP()        const;
//     float        getAttackDamage() const;
//     sf::Vector2f getPosition()     const;

//     // ── Assigns a fixed orbit slot around the tower ──────────────
//     // Fixes the "enemies walk off screen" bug by clamping each
//     // attacker to its own position on a ring around the tower.
//     void assignAttackSlot(sf::Vector2f towerPos, int slotIndex, int totalSlots);

//     // ── Separation: pushes enemies apart ─────────────────────────
//     void separate(const std::vector<std::unique_ptr<Enemy>>& all, int myIndex);

// private:
//     float hp, maxHp, speed;
//     float attackDamage;

//     std::vector<sf::Vector2f> path;
//     int  waypointIndex;
//     bool reachedEnd;
//     bool attacking;

//     // ── Attack slot ───────────────────────────────────────────────
//     sf::Vector2f attackSlotPos;     // world position this enemy orbits
//     bool         slotAssigned;

//     // ── Animation ─────────────────────────────────────────────────
//     AnimState    animState;
//     sf::Texture  walkTex, attackTex, deathTex;
//     int          frameIndex;
//     float        frameTimer;
//     float        frameSpeed;
//     bool         deathDone;
//     static const int FRAME_W     = 48;
//     static const int FRAME_H     = 48;
//     static const int FRAME_COUNT = 6;

//     void updateAnimation(float dt);
//     void setAnimState(AnimState s);
//     void applyFrame();

//     // ── Health bar ────────────────────────────────────────────────
//     sf::RectangleShape hpBarBg;
//     sf::RectangleShape hpBarFill;
//     void drawHealthBar(sf::RenderWindow& window);

//     void moveAlongPath(float dt);
// };


#pragma once
#include "Entity.h"
#include <vector>
#include <memory>
#include <SFML/Graphics.hpp>

enum class AnimState { WALK, ATTACK, DEATH };

class Enemy : public Entity
{
public:
    Enemy(const std::vector<sf::Vector2f>& path,
          float hp, float speed, float attackDamage);

    void update(float dt) override;
    void draw(sf::RenderWindow& window) override;

    void takeDamage(float amount);

    bool         isDead()          const;
    bool         hasReachedEnd()   const;
    bool         isAttacking()     const;
    bool         isDeathDone()     const;
    float        getHP()           const;
    float        getMaxHP()        const;
    float        getAttackDamage() const;
    sf::Vector2f getPosition()     const;

    // ── Used by Game::removeDeadEnemies to count each kill exactly once
    bool killCounted = false;

    void assignAttackSlot(sf::Vector2f towerPos, int slotIndex, int totalSlots);
    void separate(const std::vector<std::unique_ptr<Enemy>>& all, int myIndex);

private:
    float hp, maxHp, speed;
    float attackDamage;

    std::vector<sf::Vector2f> path;
    int  waypointIndex;
    bool reachedEnd;
    bool attacking;

    sf::Vector2f attackSlotPos;
    bool         slotAssigned;

    AnimState    animState;
    sf::Texture  walkTex, attackTex, deathTex;
    int          frameIndex;
    float        frameTimer;
    float        frameSpeed;
    bool         deathDone;
    static const int FRAME_W     = 48;
    static const int FRAME_H     = 48;
    static const int FRAME_COUNT = 6;

    void updateAnimation(float dt);
    void setAnimState(AnimState s);
    void applyFrame();

    sf::RectangleShape hpBarBg;
    sf::RectangleShape hpBarFill;
    void drawHealthBar(sf::RenderWindow& window);

    void moveAlongPath(float dt);
};
