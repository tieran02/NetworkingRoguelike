#include "Entity.h"
#include "shared/Utility/Math.h"
#include "shared/Utility/Log.h"
#include "Networking/ServerConnection.h"
#include "Graphics/SpriteManager.h"

Entity::Entity(const std::string& spriteName, CollisionLayer layer)
{
	m_sprite = SpriteManager::Instance().CreateSprite(spriteName);
	sf::FloatRect spriteBounds = m_sprite->getGlobalBounds();
	m_sprite->setOrigin(spriteBounds.width / 2.0f, spriteBounds.height / 2.0f);

	//create collider
	m_collider = std::make_shared<Collider>(m_position,sf::Vector2f{ m_sprite->getGlobalBounds().width,m_sprite->getGlobalBounds().height},this,layer);
}

Entity::~Entity()
{
}

void Entity::SetPosition(const sf::Vector2f& position, bool serverAuth)
{
	if (hasOwnership() && !serverAuth)
	{
		m_lastPosition = m_position;
		m_sprite->setPosition(position);
		m_collider->SetPosition(position);
		m_position = position;


		//only send if moved beyond threshold
		const float distance = getDistanceFromNetworkPosition();
		constexpr float threshold = 10.0f;
		if (distance >= threshold)
		{
			//send to server
			m_connection->SendMovementMessage(m_worldID, m_position, m_velocity);
		}
	}
	if(serverAuth)
	{
		m_lastPosition = m_position;
		m_sprite->setPosition(position);
		m_collider->SetPosition(position);
		m_position = position;

		//set network position
		m_lastNetworkPosition = m_networkPosition;
		m_networkPosition = position;
	}
}

sf::Vector2f Entity::GetDirection() const
{
	return Math::Normalise(m_velocity);
}

void Entity::SetVelocity(const sf::Vector2f& velocity, bool serverAuth)
{
	if (hasOwnership() && !serverAuth && m_velocity != velocity)
	{
		m_velocity = velocity;
		//send movement message to server
		m_connection->SendMovementMessage(m_worldID, m_position, m_velocity);
	}
	if(serverAuth) // velocity change came from the server so update velocity to match the server
	{
		m_velocity = velocity;
		m_networkVelocity = velocity;
	}
}

sf::Vector2f Entity::GetVelocity() const
{
	return m_velocity;
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
	if(serverAuth)
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
		m_connection->SendHealthMessage(m_worldID,health,m_maxHealth);

		//set health locally (may change when the server sends the health back)
		m_health = health;
		//disable if health is less than zero
		if(m_health <= 0.0f)
		{
			SetActive(false, serverAuth);
		}
		else
		{
			SetActive(true, serverAuth);
		}
	}
	if(serverAuth) // message came directly from the server, set values from the server values
	{
		//set health from server
		m_health = health;
		//disable if health is less than zero
		if (m_health <= 0.0f)
		{
			SetActive(false, serverAuth);
		}else
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
	if(serverAuth)
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

	m_lastPosition = m_position;
	m_position += (m_velocity * deltaTime);
	m_sprite->setPosition(m_position);
	m_collider->SetPosition(m_position);
}

void Entity::UpdatePredictedPosition(float deltaTime)
{
	if (m_position != m_networkPosition)
	{
		sf::Vector2f newPos = Math::MoveTowards(m_position, m_networkPosition, deltaTime * m_movementSpeed);
		m_lastPosition = m_position;
		m_position = newPos;
		m_sprite->setPosition(m_position);
		m_collider->SetPosition(m_position);
	}
}


void Entity::Translate(const sf::Vector2f& position)
{
	SetPosition(m_position + position,false);
}

bool Entity::hasOwnership() const
{
	return m_ownership == m_connection->GetColientID();
}

float Entity::getDistanceFromNetworkPosition() const
{
	return Math::Distance(m_position, m_networkPosition);
}
