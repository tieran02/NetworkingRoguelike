#pragma once
#include "Entity.h"
class Skeleton : public Entity
{
public:
	Skeleton();
	~Skeleton();
	void Start() override;
	void Update(float deltaTime) override;
	void Draw(sf::RenderWindow& window) override;
	void OnCollision(Collider& other) override;
	void OnLayerOverride(CollisionLayer layer) override;

	std::shared_ptr<Entity> Clone(unsigned worldID, unsigned ownership, ServerConnection* connection, World* world) override;
private:

};

