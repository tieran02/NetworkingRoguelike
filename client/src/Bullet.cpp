#include "Bullet.h"
#include "World.h"

Bullet::Bullet() : Entity("Bullet")
{
	SetMovementSpeed(400.0f);
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

void Bullet::Draw(sf::RenderWindow& window)
{
	window.draw(*GetSprite());
}

void Bullet::OnCollision(Collider& other)
{
	m_world->RequestDestroyEntity(m_worldID);
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
