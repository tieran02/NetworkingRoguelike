#include "Entity.h"

Entity::Entity(const std::string& entityName, unsigned worldID, sf::Vector2f pos, sf::Vector2f velocity, unsigned ownership, CollisionLayer layerOveride) :
	m_worldID(worldID),
	m_position(pos),
	m_lastPosition(pos),
	m_lastSentPosition(pos),
	m_velocity(velocity),
	m_ownershipID(ownership),
	m_active(true)
{
	//TODO load all entities
	m_baseData = EntityDataManager::Instance().GetEntityData(entityName);
	m_collider = std::make_shared<Collider>(m_position, sf::Vector2f{ (float)m_baseData.SpriteWidth,(float)m_baseData.SpriteHeight }, this, (CollisionLayer)m_baseData.Layer);
}

void Entity::ApplyVelocity(float deltaTime)
{
	m_lastPosition = m_position;
	m_position += (m_velocity * deltaTime);
	m_collider->SetPosition(m_position);
}

void Entity::Move(sf::Vector2f amount)
{
	m_lastPosition = m_position;
	m_position += amount;
	m_collider->SetPosition(m_position);
}

void Entity::SetPosition(sf::Vector2f pos)
{
	m_lastPosition = m_position;
	m_position = pos; 
	m_collider->SetPosition(pos);
}

void Entity::SetLastPosition(sf::Vector2f pos)
{
	m_lastPosition = pos;
}

void Entity::SetLastSentPosition(sf::Vector2f pos)
{
	m_lastSentPosition = pos;
}

void Entity::SetVelocity(sf::Vector2f velocity)
{
	m_velocity = velocity;
}

void Entity::SetActive(bool active)
{
	m_active = active;
	m_collider->SetActive(active);
}

void Entity::SetHealth(float amount)
{
	m_baseData.Health = std::min(m_baseData.MaxHealth, amount);
}

void Entity::SetMaxHealth(float amount)
{
	m_baseData.MaxHealth = amount;
}

void Entity::SetTarget(std::shared_ptr<Entity> target)
{
	m_target = target;
}

void Entity::Damage(float damage)
{
	//set health from server
	m_baseData.Health -= damage;
	//disable if health is less than zero
	if (m_baseData.Health <= 0.0f)
	{
		m_active = false;
	}
	else
	{
		m_active = true;
	}
}

void Entity::Heal(float amount)
{
	//set health from server
	m_baseData.Health -= amount;
	//disable if health is less than zero
	if (m_baseData.Health <= 0.0f)
	{
		m_active = false;
	}
	else
	{
		m_active = true;
	}
}