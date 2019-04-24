#include "Entity.h"
#include "shared/Utility/Math.h"
#include "shared/Utility/Log.h"
#include "Networking/ServerConnection.h"
#include "Graphics/SpriteManager.h"
#include "shared/EntityDataManager.h"

Entity::Entity(const std::string& entityName)
{
	const auto& baseData = EntityDataManager::Instance().GetEntityData(entityName);
	m_sprite = SpriteManager::Instance().CreateSprite(baseData.EntitySpriteName);
	sf::FloatRect spriteBounds = m_sprite->getGlobalBounds();
	m_sprite->setOrigin(m_sprite->getGlobalBounds().width / 2.0f, m_sprite->getGlobalBounds().height / 2.0f);

	m_maxHealth = baseData.MaxHealth;
	m_health = m_maxHealth;
	SetMovementSpeed(baseData.MovementSpeed);

	//create collider
	m_collider = std::make_shared<Collider>(m_position,sf::Vector2f{ (float)baseData.SpriteWidth,(float)baseData.SpriteHeight},this, baseData.Layer);
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

void Entity::SetLastNetworkPosition(const sf::Vector2f& position)
{
	m_lastNetworkPosition = position;
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

void Entity::SetMovementSpeed(float speed)
{
	m_movementSpeed = speed;
}

float Entity::GetMovementSpeed() const
{
	return m_movementSpeed;
}

void Entity::SetActive(bool active, bool serverAuth)
{
	if (hasOwnership() && active != m_active && !serverAuth)
	{
		m_active = active;
		m_collider->SetActive(active);
		//send active only if this message didn't come from the server
		m_connection->SendEntityStateMessage(*this);
	}
	if (serverAuth)
	{
		m_active = active;
		m_collider->SetActive(active);
	}
}

bool Entity::IsActive() const
{
	return m_active;
}

void Entity::SetHealth(float health, bool serverAuth)
{
	if (hasOwnership() && health != m_health && !serverAuth)
	{
		//send health message to server
		m_connection->SendHealthMessage(m_worldID, health, m_maxHealth);

		//set health locally (may change when the server sends the health back)
		m_health = health;
		//disable if health is less than zero
		if (m_health <= 0.0f)
		{
			SetActive(false, serverAuth);
		}
		else
		{
			SetActive(true, serverAuth);
		}
	}
	if (serverAuth) // message came directly from the server, set values from the server values
	{
		//set health from server
		m_health = health;
		//disable if health is less than zero
		if (m_health <= 0.0f)
		{
			SetActive(false, serverAuth);
		}
		else
		{
			SetActive(true, serverAuth);
		}
	}
}

float Entity::GetHealth() const
{
	return m_health;
}

void Entity::SetMaxHealth(float health, bool serverAuth)
{
	if (hasOwnership() && health != m_maxHealth && !serverAuth)
	{
		//send health message to server
		m_connection->SendHealthMessage(m_worldID, health, m_maxHealth);

		//set health locally (may change when the server sends authorized health back)
		m_maxHealth = health;
	}
	if (serverAuth)
	{
		m_maxHealth = health;
	}
}

float Entity::GetMaxHealth() const
{
	return m_maxHealth;
}

void Entity::Damage(float amount)
{
	SetHealth(m_health - amount, false);
}

void Entity::Heal(float amount)
{
	SetHealth(m_health + amount, false);
}

sf::Vector2f Entity::CalculatePredictedPosition() const
{
	return m_position + m_velocity;
}

void Entity::UpdatePosition(float deltaTime)
{
	if (Math::SqrMagnitude(m_velocity) == 0.0f)
		return;

	SetPosition(m_position + (m_velocity * deltaTime));
}

void Entity::UpdatePredictedPosition(float deltaTime)
{
	if (Math::Distance(m_position, m_networkPosition) <= 0.5f)
		return;

	sf::Vector2f newPos = Math::MoveTowards(m_lastNetworkPosition, m_networkPosition, deltaTime * m_movementSpeed);
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
