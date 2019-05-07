#include "SingleshotPickup.h"
#include "World.h"

SingleshotPickup::SingleshotPickup() : Entity("SingleshotPickup")
{
	GetCollider()->SetCollideMask(Collider::AllLayers() & ~(CollisionLayer::PROJECTILE_ENEMY | CollisionLayer::PROJECTILE_NEUTRAL | CollisionLayer::PROJECTILE_PLAYER | CollisionLayer::ENEMY));
}

SingleshotPickup::~SingleshotPickup()
{
}

void SingleshotPickup::Start()
{

}

void SingleshotPickup::Update(float deltaTime)
{
}

void SingleshotPickup::OnCollision(Collider& other)
{
	//pickup

	m_world->RequestDestroyEntity(m_worldID);
}

void SingleshotPickup::OnLayerOverride(CollisionLayer layer)
{

}

std::shared_ptr<Entity> SingleshotPickup::Clone(unsigned worldID, unsigned ownership, ServerConnection* connection, World* world)
{
	auto pickup = std::make_shared<SingleshotPickup>();
	pickup->m_worldID = worldID;
	pickup->m_ownership = ownership;
	pickup->m_connection = connection;
	pickup->m_world = world;
	return pickup;
}
