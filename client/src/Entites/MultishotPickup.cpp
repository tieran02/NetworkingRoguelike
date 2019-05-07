#include "MultishotPickup.h"
#include "World.h"
#include "Player.h"

MultishotPickup::MultishotPickup() : Entity("MultishotPickup")
{
	GetCollider()->SetCollideMask(Collider::AllLayers() & ~(CollisionLayer::PROJECTILE_ENEMY | CollisionLayer::PROJECTILE_NEUTRAL | CollisionLayer::PROJECTILE_PLAYER | CollisionLayer::ENEMY));
}

MultishotPickup::~MultishotPickup()
{
}

void MultishotPickup::Start()
{

}

void MultishotPickup::Update(float deltaTime)
{
}

void MultishotPickup::OnCollision(Collider& other)
{
	//pickup
	Entity* entity = other.GetEntity();
	if (entity != nullptr && entity->GetEntityID() == 0)
	{
		Player* player = (Player*)entity;
		//create random cone weapon
		player->SetWeapon(Weapon::CreateWeapon(AreaOfAttack::CIRCLE));
	}

	m_world->RequestDestroyEntity(m_worldID);
}

void MultishotPickup::OnLayerOverride(CollisionLayer layer)
{

}

std::shared_ptr<Entity> MultishotPickup::Clone(unsigned worldID, unsigned ownership, ServerConnection* connection, World* world)
{
	auto pickup = std::make_shared<MultishotPickup>();
	pickup->m_worldID = worldID;
	pickup->m_ownership = ownership;
	pickup->m_connection = connection;
	pickup->m_world = world;
	return pickup;
}
