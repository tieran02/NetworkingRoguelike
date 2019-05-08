#pragma once
#include "Entity.h"

class ConeshotPickup : public Entity
{
public:
	ConeshotPickup();
	~ConeshotPickup() override;
	void Start() override;
	void Update(float deltaTime) override;
	void OnCollision(Collider& other) override;
	void OnLayerOverride(CollisionLayer layer) override;


	std::shared_ptr<Entity> Clone(unsigned int worldID, unsigned int ownership, ServerConnection* connection, World* world) override;
private:
	
};
