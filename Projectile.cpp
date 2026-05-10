#include "Projectile.h"
#include <cmath>
#include <iostream>

// arrow.png is 32x32, points RIGHT naturally.
// flyingRight=true  → no flip  (arrow.png natural direction = right)
// flyingRight=false → flip x   (mirror = points left)

Projectile::Projectile(sf::Vector2f origin,
                       sf::Vector2f target,
                       bool         flyingRight,
                       float        damage,
                       int          id)
    : position(origin),
      target(target),
      flyingRight(flyingRight),
      damage(damage), id(id),
      speed(500.f),
      expired(false)
{
    if (!texture.loadFromFile("assets/arrow.png"))
    {
        std::cerr << "Projectile: could not load assets/arrow.png\n";
    }

    sprite.setTexture(texture);
    sprite.setOrigin(16.f, 16.f);   // centre of 32x32 sprite

    // arrow.png points RIGHT.
    // flyingRight=true  → scale(+2, 2) = natural right
    // flyingRight=false → scale(-2, 2) = mirrored = left
    float sx = flyingRight ? 2.f : -2.f;
    sprite.setScale(sx, 2.f);
    sprite.setPosition(position);

    // Velocity: purely horizontal in the facing direction
    sf::Vector2f diff = target - origin;
    float dist = std::sqrt(diff.x*diff.x + diff.y*diff.y);
    if (dist > 0.f)
        velocity = (diff / dist) * speed;
}

void Projectile::update(float dt)
{
    if (expired) return;

    position += velocity * dt;
    sprite.setPosition(position);

    if (hasReachedTarget())
        expired = true;
}

bool Projectile::hasReachedTarget() const
{
    sf::Vector2f diff = target - position;
    return std::sqrt(diff.x*diff.x + diff.y*diff.y) < 12.f;
}

void Projectile::expire() { expired = true; }

void Projectile::draw(sf::RenderWindow& window)
{
    if (expired) return;

    if (texture.getSize().x == 0)
    {
        // Fallback dot if arrow.png missing
        sf::CircleShape dot(7.f);
        dot.setFillColor(sf::Color(255, 210, 30));
        dot.setOrigin(7.f, 7.f);
        dot.setPosition(position);
        window.draw(dot);
        return;
    }

    window.draw(sprite);
}

bool         Projectile::isExpired()   const { return expired; }
float        Projectile::getDamage()   const { return damage; }
sf::Vector2f Projectile::getPosition() const { return position; }
