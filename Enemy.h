
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
