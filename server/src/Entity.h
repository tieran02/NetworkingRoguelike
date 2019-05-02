#include <string>
#include <SFML/System/Vector2.hpp>
#include "shared/Collider.h"
#include "shared/EntityDataManager.h"
#include <memory>

class Entity
{
public:
	Entity(const std::string& entityName, unsigned int worldID, sf::Vector2f pos, sf::Vector2f velocity, unsigned int ownership, CollisionLayer layerOveride);
	Entity() {}

	const EntityBaseData& BaseData() const { return m_baseData; }
	const unsigned int& WorldID() const { return m_worldID; }
	const sf::Vector2f& Position() const { return m_position; }
	const sf::Vector2f& LastPosition() const { return m_lastPosition; }
	const sf::Vector2f& LastSentPosition() const { return m_lastSentPosition; }
	const sf::Vector2f& Velocity() const { return m_velocity; }
	const unsigned int& OwnershipID() const { return m_ownershipID; }
	const bool& IsActive() const { return m_active; }
	std::shared_ptr<Collider> GetCollider() { return m_collider; }
	std::shared_ptr<Entity> GetTarget() const { return m_target; }

	void ApplyVelocity(float deltaTime);

	void Move(sf::Vector2f amount);
	void SetPosition(sf::Vector2f pos);
	void SetLastPosition(sf::Vector2f pos);
	void SetLastSentPosition(sf::Vector2f pos);
	void SetVelocity(sf::Vector2f velocity);
	void SetActive(bool active);
	void SetHealth(float amount);
	void SetMaxHealth(float amount);
	void Damage(float damage);
	void Heal(float amount);

	void SetTarget(std::shared_ptr<Entity> target);
private:
	EntityBaseData m_baseData;
	unsigned int m_worldID;
	sf::Vector2f m_position;
	sf::Vector2f m_lastPosition;
	sf::Vector2f m_lastSentPosition;

	sf::Vector2f m_velocity;
	unsigned int m_ownershipID;
	bool m_active;
	std::shared_ptr<Collider> m_collider;
	std::shared_ptr<Entity> m_target{nullptr}; //target for enemies
};
