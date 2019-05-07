#include "ConeshotPickup.h"
#include "World.h"
#include "Player.h"

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
	Entity* entity = other.GetEntity();
	if (entity != nullptr && entity->GetEntityID() == 0)
	{
		Player* player = (Player*)entity;
		//create random cone weapon
		player->SetWeapon(Weapon::CreateWeapon(AreaOfAttack::CONE));
	}


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
