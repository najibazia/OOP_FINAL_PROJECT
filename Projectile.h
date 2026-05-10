

#pragma once
#include "Entity.h"
#include <SFML/Graphics.hpp>

class Projectile : public Entity
{
public:
    Projectile(sf::Vector2f origin,
               sf::Vector2f target,
               bool         flyingRight,
               float        damage,
               int          id);

    void update(float dt) override;
    void draw(sf::RenderWindow& window) override;

    void expire();

    bool         isExpired()   const;
    float        getDamage()   const;
    sf::Vector2f getPosition() const;

private:
    int   id;
    float damage;
    bool  flyingRight;

    sf::Vector2f position;
    sf::Vector2f target;
    sf::Vector2f velocity;

    float speed;
    bool  expired;

    sf::Texture texture;
    sf::Sprite  sprite;

    bool hasReachedTarget() const;
};
