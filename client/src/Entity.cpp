#include "Entity.h"
#include "Networking/ServerConnection.h"
#include "shared/Utility/Math.h"
#include "shared/MovementMessage.h"

Entity::Entity()
{
    //ctor
}

Entity::~Entity()
{
    //dtor
}

void Entity::SetPosition(const sf::Vector2f& position)
{
	m_lastPosition = m_position;

    m_sprite.setPosition(position);
    m_position = position;
}

void Entity::SetNetworkPosition(const sf::Vector2f& position)
{
	m_networkPosition = position;
}

sf::Vector2f Entity::GetDirection() const
{
	return Math::Normalise(m_velocity);
}

void Entity::SetVelocity(sf::Vector2f velocity)
{
	m_velocity = velocity;
}

sf::Vector2f Entity::GetVelocity() const
{
	return m_velocity;
}

void Entity::SetNetworkVelocity(const sf::Vector2f& velocity)
{
	m_networkVelocity = velocity;
}

void Entity::UpdatePosition()
{
	m_lastPosition = m_position;
	m_position += m_velocity;
}


void Entity::Translate(const sf::Vector2f& position)
{
	m_lastPosition = m_position;

	m_position += position;
	m_sprite.setPosition(m_position);
}

bool Entity::hasOwnership() const
{
	return m_ownership == m_connection->GetColientID();
}
