#pragma once
#include <SFML/Graphics.hpp>
#include <memory>

class Entity
{
public:
    Entity();
    virtual ~Entity();

    virtual void Start() = 0;
    virtual void Update() = 0;
    virtual void Draw() = 0;
    virtual std::shared_ptr<Entity> Clone() = 0;
protected:
    sf::Sprite m_sprite;
    sf::Vector2f m_position;
};
