#include "Player.h"
#include <cmath>
#include <iostream>


Player::Player(sf::Vector2f startPos,
               sf::Vector2f towerPos,
               float mapW, float mapH)
    : towerPos(towerPos),
      mapW(mapW), mapH(mapH),
      state(PlayerState::IDLE),
      facingRight(false),         
      attackCooldown(0.4f),
      attackTimer(0.4f),
      nextArrowID(0),
      frameIndex(0),
      frameTimer(0.f),
      frameSpeed(0.10f)
{
    position = startPos;

    if (!idleTex.loadFromFile("assets/player_idle.png"))
        std::cerr << "Player: could not load assets/player_idle.png\n";
    if (!attackTex.loadFromFile("assets/player_attack.png"))
        std::cerr << "Player: could not load assets/player_attack.png\n";

    shape.setTexture(idleTex);
    shape.setTextureRect(sf::IntRect(0, 0, FRAME_W, FRAME_H));
    shape.setOrigin(FRAME_W / 2.f, FRAME_H / 2.f);
    shape.setScale(2.f, 2.f);     // start facing left (natural)
    shape.setPosition(position);
}

void Player::applyFacing()
{
    float sx = facingRight ? -2.f : 2.f;
    shape.setScale(sx, 2.f);
}
void Player::update(float dt)
{
    attackTimer += dt;

    updateMovement(dt);
    updateAnimation(dt);

    if (state == PlayerState::ATTACK && frameIndex >= FRAME_COUNT - 1)
    {
        state      = PlayerState::IDLE;
        frameIndex = 0;
        frameTimer = 0.f;
        shape.setTexture(idleTex);
    }

    applyFacing();
    shape.setPosition(position);
}

void Player::updateMovement(float dt)
{
    sf::Vector2f dir(0.f, 0.f);

    bool leftHeld  = sf::Keyboard::isKeyPressed(sf::Keyboard::Left)  ||
                     sf::Keyboard::isKeyPressed(sf::Keyboard::A);
    bool rightHeld = sf::Keyboard::isKeyPressed(sf::Keyboard::Right) ||
                     sf::Keyboard::isKeyPressed(sf::Keyboard::D);
    bool upHeld    = sf::Keyboard::isKeyPressed(sf::Keyboard::Up)    ||
                     sf::Keyboard::isKeyPressed(sf::Keyboard::W);
    bool downHeld  = sf::Keyboard::isKeyPressed(sf::Keyboard::Down)  ||
                     sf::Keyboard::isKeyPressed(sf::Keyboard::S);

    if (leftHeld)  { dir.x -= 1.f; facingRight = false; }  // face left
    if (rightHeld) { dir.x += 1.f; facingRight = true;  }  // face right
    if (upHeld)    dir.y -= 1.f;
    if (downHeld)  dir.y += 1.f;

    float len = std::sqrt(dir.x * dir.x + dir.y * dir.y);
    if (len > 0.f)
    {
        dir /= len;
        position += dir * MOVE_SPEED * dt;
    }

    clampToMap();
    blockTower();
}

std::unique_ptr<Projectile> Player::shoot()
{
    if (attackTimer < attackCooldown)
        return nullptr;

    attackTimer = 0.f;

    state      = PlayerState::ATTACK;
    frameIndex = 0;
    frameTimer = 0.f;
    shape.setTexture(attackTex);
    applyFacing();

    float        dx     = facingRight ? ARROW_RANGE : -ARROW_RANGE;
    sf::Vector2f target = { position.x + dx, position.y };

    return std::make_unique<Projectile>(
        position,
        target,
        facingRight,   
        40.f,
        nextArrowID++);
}

void Player::clampToMap()
{
    const float HALF = (float)FRAME_W;
    if (position.x < HALF)        position.x = HALF;
    if (position.x > mapW - HALF) position.x = mapW - HALF;
    if (position.y < HALF)        position.y = HALF;
    if (position.y > mapH - HALF) position.y = mapH - HALF;
}

void Player::blockTower()
{
    sf::Vector2f diff = position - towerPos;
    float dist = std::sqrt(diff.x * diff.x + diff.y * diff.y);
    if (dist < TOWER_BLOCK_R && dist > 0.1f)
        position = towerPos + (diff / dist) * TOWER_BLOCK_R;
}

void Player::updateAnimation(float dt)
{
    frameTimer += dt;
    if (frameTimer < frameSpeed) return;
    frameTimer = 0.f;
    frameIndex = (frameIndex + 1) % FRAME_COUNT;
    shape.setTextureRect(
        sf::IntRect(frameIndex * FRAME_W, 0, FRAME_W, FRAME_H));
}

void Player::draw(sf::RenderWindow& window) { window.draw(shape); }
sf::Vector2f Player::getPosition() const { return position; }
