#include "ConeshotPickup.h"
#include "World.h"

ConeshotPickup::ConeshotPickup() : Entity("ConeshotPickup")
{
	GetCollider()->SetCollideMask(Collider::AllLayers() & ~(CollisionLayer::PROJECTILE_ENEMY | CollisionLayer::PROJECTILE_NEUTRAL | CollisionLayer::PROJECTILE_PLAYER | CollisionLayer::ENEMY));
}

ConeshotPickup::~ConeshotPickup()
{
}

void ConeshotPickup::Start()
{

}

void ConeshotPickup::Update(float deltaTime)
{
}

void ConeshotPickup::OnCollision(Collider& other)
{
	//pickup

	m_world->RequestDestroyEntity(m_worldID);
}

void ConeshotPickup::OnLayerOverride(CollisionLayer layer)
{

}

std::shared_ptr<Entity> ConeshotPickup::Clone(unsigned worldID, unsigned ownership, ServerConnection* connection, World* world)
{
	auto pickup = std::make_shared<ConeshotPickup>();
	pickup->m_worldID = worldID;
	pickup->m_ownership = ownership;
	pickup->m_connection = connection;
	pickup->m_world = world;
	return pickup;
}
