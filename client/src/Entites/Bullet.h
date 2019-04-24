#pragma once
#include "Entity.h"

class Bullet : public Entity
{
public:
	Bullet();
	~Bullet() override;
	void Start() override;
	void Update(float deltaTime) override;
	void Draw(sf::RenderWindow & window) override;
	void OnCollision(Collider& other) override;
	void OnLayerOverride(CollisionLayer layer) override;


	std::shared_ptr<Entity> Clone(unsigned int worldID, unsigned int ownership, ServerConnection* connection, World* world) override;
private:
	float m_damage{ 10.0f };
};
