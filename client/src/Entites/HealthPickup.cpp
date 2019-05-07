#include "HealthPickup.h"
#include "World.h"

HealthPickup::HealthPickup() : Entity("HealthPickup")
{
	GetCollider()->SetCollideMask(Collider::AllLayers() & ~(CollisionLayer::PROJECTILE_ENEMY | CollisionLayer::PROJECTILE_NEUTRAL | CollisionLayer::PROJECTILE_PLAYER | CollisionLayer::ENEMY));
}

HealthPickup::~HealthPickup()
{
}

void HealthPickup::Start()
{

}

void HealthPickup::Update(float deltaTime)
{
}

void HealthPickup::OnCollision(Collider& other)
{
	//pickup

	m_world->RequestDestroyEntity(m_worldID);
}

void HealthPickup::OnLayerOverride(CollisionLayer layer)
{

}

std::shared_ptr<Entity> HealthPickup::Clone(unsigned worldID, unsigned ownership, ServerConnection* connection, World* world)
{
	auto pickup = std::make_shared<HealthPickup>();
	pickup->m_worldID = worldID;
	pickup->m_ownership = ownership;
	pickup->m_connection = connection;
	pickup->m_world = world;
	return pickup;
}
