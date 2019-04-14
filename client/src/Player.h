#pragma once
#include "Entity.h"

class Player : public Entity
{
public:
    Player();
    ~Player() override;
    void Start() override;
    void Update() override;
    void Draw(sf::RenderWindow & window) override;

    std::shared_ptr<Entity> Clone(unsigned int worldID, unsigned int ownership, ServerConnection* connection, World* world) override;
};
