#include "Entity.h"
#include "shared/Utility/Math.h"
#include "shared/Utility/Log.h"
#include "Networking/ServerConnection.h"
#include "Graphics/SpriteManager.h"

Entity::Entity(const std::string& spriteName)
{
	m_sprite = SpriteManager::Instance().CreateSprite(spriteName);
	m_collider = std::make_shared<Collider>(m_position,sf::Vector2f{ m_sprite->getGlobalBounds().width,m_sprite->getGlobalBounds().height});
}

Entity::~Entity()
{
}

void Entity::SetPosition(const sf::Vector2f& position)
{
	m_lastPosition = m_position;

    m_sprite->setPosition(position);
	m_collider->SetPosition(position);
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

sf::Vector2f Entity::CalculatePredictedPosition() const
{
	return m_position + m_velocity;
}

void Entity::UpdatePosition(float deltaTime)
{
	if (Math::SqrMagnitude(m_velocity) == 0.0f)
		return;

	SetPosition(m_position + m_velocity);
}

void Entity::UpdatePredictedPosition(float deltaTime)
{
	if (Math::Distance(m_position, m_networkPosition) <= 0.5f)
		return;

	sf::Vector2f newPos = Math::MoveTowards(m_position, m_networkPosition, deltaTime * m_movementSpeed);
	SetPosition(newPos);
}


void Entity::Translate(const sf::Vector2f& position)
{
	SetPosition(m_position + position);
}

bool Entity::hasOwnership() const
{
	return m_ownership == m_connection->GetColientID();
}
