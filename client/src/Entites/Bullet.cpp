#include "Bullet.h"
#include "World.h"

Bullet::Bullet() : Entity("Bullet")
{
	GetCollider()->SetCollideMask(Collider::AllLayers() & ~(CollisionLayer::PROJECTILE_NEUTRAL | CollisionLayer::PICKUP));
}

Bullet::~Bullet()
{
}

void Bullet::Start()
{

}

void Bullet::Update(float deltaTime)
{
	UpdatePosition(deltaTime);
}

void Bullet::OnCollision(Collider& other)
{
	Entity* entity = other.GetEntity();
	if (entity != nullptr )
	{
		entity->Damage(m_damage);
	}

	m_world->RequestDestroyEntity(m_worldID);
}

void Bullet::OnLayerOverride(CollisionLayer layer)
{
	//make sure bullets don't collide with self
	GetCollider()->SetCollideMask(Collider::AllLayers() & ~(layer | CollisionLayer::PROJECTILE_ENEMY | CollisionLayer::PROJECTILE_NEUTRAL | CollisionLayer::PROJECTILE_PLAYER  | CollisionLayer::PICKUP));
}

std::shared_ptr<Entity> Bullet::Clone(unsigned worldID, unsigned ownership, ServerConnection* connection, World* world)
{
	auto bullet = std::make_shared<Bullet>();
	bullet->m_worldID = worldID;
	bullet->m_ownership = ownership;
	bullet->m_connection = connection;
	bullet->m_world = world;
	return bullet;
}
