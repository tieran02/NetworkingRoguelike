#include "Skeleton.h"

Skeleton::Skeleton() : Entity("Skeleton")
{
	//collide with everything except player projectiles
	GetCollider()->SetCollideMask(Collider::AllLayers() & ~(CollisionLayer::PROJECTILE_ENEMY));
}


Skeleton::~Skeleton()
{
}

void Skeleton::Start()
{
}

void Skeleton::Update(float deltaTime)
{
	UpdatePredictedPosition(deltaTime);
}

void Skeleton::OnCollision(Collider& other)
{
}

void Skeleton::OnLayerOverride(CollisionLayer layer)
{
}

std::shared_ptr<Entity> Skeleton::Clone(unsigned worldID, unsigned ownership, ServerConnection* connection, World* world)
{
	auto skeleton = std::make_shared<Skeleton>();
	skeleton->m_worldID = worldID;
	skeleton->m_ownership = ownership;
	skeleton->m_connection = connection;
	skeleton->m_world = world;
	return skeleton;
}
