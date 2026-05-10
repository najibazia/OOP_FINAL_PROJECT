
#include <SFML/Graphics.hpp>

class Entity
{
protected:
    sf::Vector2f position;
    sf::Texture  texture;
    sf::Sprite   shape;

public:
    Entity() = default;

    Entity(const Entity& other)
        : position(other.position),
          texture(other.texture)
    {
        shape = other.shape;
        shape.setTexture(texture);
    }

    virtual ~Entity() = default;

    virtual void update(float dt)               = 0;  
    virtual void draw(sf::RenderWindow& window) = 0; 

    virtual sf::Vector2f getPosition() const { return position; }
};
