#include "Entity.h"
#include "Networking/ServerConnection.h"

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
    m_sprite.setPosition(position);
    m_position = position;
}

void Entity::SetNetworkPosition(const sf::Vector2f& position)
{
	m_networkPosition = position;
}

void Entity::Translate(const sf::Vector2f& position)
{
	m_position += position;
	m_sprite.setPosition(m_position);
}

bool Entity::hasOwnership() const
{
	return m_ownership == m_connection->GetColientID();
}
