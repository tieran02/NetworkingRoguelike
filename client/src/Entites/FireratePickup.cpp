#include "FireratePickup.h"
#include "World.h"

FireratePickup::FireratePickup() : Entity("FireratePickup")
{
	GetCollider()->SetCollideMask(Collider::AllLayers() & ~(CollisionLayer::PROJECTILE_ENEMY | CollisionLayer::PROJECTILE_NEUTRAL | CollisionLayer::PROJECTILE_PLAYER | CollisionLayer::ENEMY));
}

FireratePickup::~FireratePickup()
{
}

void FireratePickup::Start()
{

}

void FireratePickup::Update(float deltaTime)
{
}

void FireratePickup::OnCollision(Collider& other)
{
	//pickup

	m_world->RequestDestroyEntity(m_worldID);
}

void FireratePickup::OnLayerOverride(CollisionLayer layer)
{

}

std::shared_ptr<Entity> FireratePickup::Clone(unsigned worldID, unsigned ownership, ServerConnection* connection, World* world)
{
	auto pickup = std::make_shared<FireratePickup>();
	pickup->m_worldID = worldID;
	pickup->m_ownership = ownership;
	pickup->m_connection = connection;
	pickup->m_world = world;
	return pickup;
}
