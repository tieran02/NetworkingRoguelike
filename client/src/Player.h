#pragma once
#include "Entity.h"

class Player : public Entity
{
public:
    Player();
    ~Player() override;
    void Start() override;
    void Update(float deltaTime) override;
    void Draw(sf::RenderWindow & window) override;
	void OnCollision(Collider& other) override;

    std::shared_ptr<Entity> Clone(unsigned int worldID, unsigned int ownership, ServerConnection* connection, World* world) override;
    
};
