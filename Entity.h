// #pragma once
// #include <SFML/Graphics.hpp>

// class Entity
// {
// protected:
//     sf::Vector2f position;
//     sf::Texture  texture;
//     sf::Sprite   shape;

// public:
//     virtual ~Entity() = default;
//     virtual void update(float dt) = 0;
//     virtual void draw(sf::RenderWindow& window) = 0;
// };
#pragma once
// ══════════════════════════════════════════════════════════════════
//  Entity.h  — Abstract Base Class
//  Concepts:
//    • Abstract Class      — has two pure virtual functions
//    • Inheritance base    — Enemy and Projectile both extend this
//    • Constructors        — default + copy constructor
//    • Encapsulation       — protected members, not public
// ══════════════════════════════════════════════════════════════════
#include <SFML/Graphics.hpp>

class Entity
{
protected:
    sf::Vector2f position;
    sf::Texture  texture;
    sf::Sprite   shape;

public:
    Entity() = default;

    // ── Copy constructor (spec requirement) ──────────────────────
    Entity(const Entity& other)
        : position(other.position),
          texture(other.texture)
    {
        // Re-attach sprite to the copied texture (sprite holds a raw pointer)
        shape = other.shape;
        shape.setTexture(texture);
    }

    virtual ~Entity() = default;

    // ── Pure virtual functions — makes Entity an Abstract Class ──
    // Any class that does not implement both of these cannot be
    // instantiated. This enforces the interface contract.
    virtual void update(float dt)               = 0;  // pure virtual
    virtual void draw(sf::RenderWindow& window) = 0;  // pure virtual

    // ── Virtual getter so subclasses can override ─────────────────
    virtual sf::Vector2f getPosition() const { return position; }
};