


#include "Enemy.h"
#include <cmath>
#include <iostream>
#include <memory>

static const float PI = 3.14159265f;

Enemy::Enemy(const std::vector<sf::Vector2f>& path,
             float hp, float speed, float attackDamage)
    : hp(hp), maxHp(hp), speed(speed),
      attackDamage(attackDamage),
      path(path), waypointIndex(0),
      reachedEnd(false), attacking(false),
      slotAssigned(false), attackSlotPos(0.f, 0.f),
      animState(AnimState::WALK),
      frameIndex(0), frameTimer(0.f),
      frameSpeed(0.12f),
      deathDone(false)
{
    if (!path.empty())
        position = path[0];

    if (!walkTex.loadFromFile("assets/walk.png"))
        std::cerr << "Warning: could not load assets/walk.png\n";
    if (!attackTex.loadFromFile("assets/attack.png"))
        std::cerr << "Warning: could not load assets/attack.png\n";
    if (!deathTex.loadFromFile("assets/death.png"))
        std::cerr << "Warning: could not load assets/death.png\n";

    shape.setTexture(walkTex);
    shape.setTextureRect(sf::IntRect(0, 0, FRAME_W, FRAME_H));
    shape.setScale(2.5f, 2.5f);
    shape.setOrigin(FRAME_W / 2.f, FRAME_H / 2.f);
    shape.setPosition(position);

    hpBarBg.setSize({ 50.f, 6.f });
    hpBarBg.setFillColor(sf::Color(180, 30, 30));
    hpBarFill.setSize({ 50.f, 6.f });
    hpBarFill.setFillColor(sf::Color(30, 200, 30));
}

void Enemy::update(float dt)
{
    if (deathDone) return;
    updateAnimation(dt);
    if (isDead())  return;

    if (attacking)
    {
        if (slotAssigned)
        {
            position = attackSlotPos;
            shape.setPosition(position);
        }
        return;
    }

    moveAlongPath(dt);
}

void Enemy::assignAttackSlot(sf::Vector2f towerPos, int slotIndex, int totalSlots)
{
    const float ORBIT_RADIUS = 55.f;
    float angle = (2.f * PI / totalSlots) * slotIndex;

    attackSlotPos = {
        towerPos.x + std::cos(angle) * ORBIT_RADIUS,
        towerPos.y + std::sin(angle) * ORBIT_RADIUS
    };
    slotAssigned = true;

    if (attacking)
    {
        position = attackSlotPos;
        shape.setPosition(position);
    }
}

void Enemy::updateAnimation(float dt)
{
    frameTimer += dt;
    if (frameTimer < frameSpeed) return;
    frameTimer = 0.f;

    if (animState == AnimState::DEATH)
    {
        if (frameIndex < FRAME_COUNT - 1)
            frameIndex++;
        else
            deathDone = true;
    }
    else
    {
        frameIndex = (frameIndex + 1) % FRAME_COUNT;
    }
    applyFrame();
}

void Enemy::setAnimState(AnimState s)
{
    if (animState == s) return;
    animState  = s;
    frameIndex = 0;
    frameTimer = 0.f;

    switch (s)
    {
        case AnimState::WALK:
            shape.setTexture(walkTex);
            frameSpeed = 0.12f;
            break;
        case AnimState::ATTACK:
            shape.setTexture(attackTex);
            frameSpeed = 0.10f;
            break;
        case AnimState::DEATH:
            shape.setTexture(deathTex);
            frameSpeed = 0.15f;
            break;
    }
    applyFrame();
}

void Enemy::applyFrame()
{
    shape.setTextureRect(
        sf::IntRect(frameIndex * FRAME_W, 0, FRAME_W, FRAME_H));
}

void Enemy::moveAlongPath(float dt)
{
    if (waypointIndex >= (int)path.size())
    {
        position   = path.back();
        reachedEnd = true;
        attacking  = true;
        setAnimState(AnimState::ATTACK);
        shape.setPosition(position);
        return;
    }

    sf::Vector2f target = path[waypointIndex];
    sf::Vector2f diff   = target - position;
    float dist = std::sqrt(diff.x*diff.x + diff.y*diff.y);

    if (dist < 2.f)
    {
        position = target;
        waypointIndex++;
    }
    else
    {
        sf::Vector2f dir = diff / dist;
        position += dir * speed * dt;
    }

    setAnimState(AnimState::WALK);
    shape.setPosition(position);
}

void Enemy::separate(const std::vector<std::unique_ptr<Enemy>>& all, int myIndex)
{
    if (attacking || isDead()) return;

    const float MIN_DIST = 55.f;
    const float PUSH_STR = 0.4f;
    sf::Vector2f push(0.f, 0.f);

    for (int i = 0; i < (int)all.size(); i++)
    {
        if (i == myIndex) continue;
        if (all[i]->isDead()) continue;

        sf::Vector2f diff = position - all[i]->position;
        float dist = std::sqrt(diff.x*diff.x + diff.y*diff.y);

        if (dist < MIN_DIST && dist > 0.1f)
        {
            sf::Vector2f dir = diff / dist;
            push += dir * (MIN_DIST - dist) * PUSH_STR;
        }
    }

    position += push;
    shape.setPosition(position);
}
void Enemy::draw(sf::RenderWindow& window)
{
    if (deathDone) return;
    window.draw(shape);
    if (!isDead())
        drawHealthBar(window);
}

void Enemy::drawHealthBar(sf::RenderWindow& window)
{
    sf::FloatRect bounds = shape.getGlobalBounds();
    sf::Vector2f  barPos = { bounds.left, bounds.top - 10.f };

    hpBarBg.setSize({ bounds.width, 6.f });
    hpBarBg.setPosition(barPos);
    window.draw(hpBarBg);

    float ratio = hp / maxHp;
    hpBarFill.setSize({ bounds.width * ratio, 6.f });
    hpBarFill.setPosition(barPos);
    window.draw(hpBarFill);
}
void Enemy::takeDamage(float amount)
{
    hp -= amount;
    if (hp < 0.f) hp = 0.f;
    if (hp <= 0.f)
        setAnimState(AnimState::DEATH);
}

bool  Enemy::isDead()          const { return hp <= 0.f; }
bool  Enemy::hasReachedEnd()   const { return reachedEnd; }
bool  Enemy::isAttacking()     const { return attacking; }
bool  Enemy::isDeathDone()     const { return deathDone; }
float Enemy::getHP()           const { return hp; }
float Enemy::getMaxHP()        const { return maxHp; }
float Enemy::getAttackDamage() const { return attackDamage; }
sf::Vector2f Enemy::getPosition() const { return position; }


