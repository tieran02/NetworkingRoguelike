#pragma once
#include <SFML/Graphics.hpp>
#include <memory>
#include "shared/Collider.h"

class World;
class ServerConnection;
class Entity
{
public:
    Entity(const std::string& spriteName, CollisionLayer layer);
    virtual ~Entity();

    virtual void Start() = 0;
    virtual void Update(float deltaTime) = 0;
    virtual void Draw(sf::RenderWindow & window) = 0;
	virtual void OnCollision(Collider& other) = 0;
	virtual void OnLayerOverride(CollisionLayer layer) = 0;
    virtual std::shared_ptr<Entity> Clone(unsigned int worldID, unsigned int ownership, ServerConnection* connection, World* world) = 0;

	unsigned int GetWorldID() const { return m_worldID; }
	const std::shared_ptr<sf::Sprite>& GetSprite() const { return m_sprite; }
    void SetPosition(const sf::Vector2f& position);
	void SetLocalPosition(const sf::Vector2f& position);

	sf::Vector2f GetPosition() const { return m_position; }
	sf::Vector2f GetLastPosition() const { return m_lastPosition; }

	void SetNetworkPosition(const sf::Vector2f& position);
	sf::Vector2f GetNetworkPosition() const { return m_networkPosition; }
	void SetLastNetworkPosition(const sf::Vector2f& position);
	sf::Vector2f GetLastNetworkPosition() const { return m_lastNetworkPosition; }

	sf::Vector2f GetDirection() const;
	void SetVelocity(sf::Vector2f velocity);
	sf::Vector2f GetVelocity() const;
	void SetNetworkVelocity(const sf::Vector2f& velocity);
	sf::Vector2f GetNetworkVelocity() const { return m_networkVelocity; }
	void SetMovementSpeed(float speed);
	float GetMovementSpeed() const;

	std::shared_ptr<Collider> GetCollider() const { return m_collider; }
	void SetActive(bool active, bool serverAuth);
	bool IsActive() const;

	void SetHealth(float health, bool serverAuth);
	float GetHealth() const;
	void SetMaxHealth(float health, bool serverAuth);
	float GetMaxHealth() const;
	void Damage(float amount);
	void Heal(float amount);

	sf::Vector2f CalculatePredictedPosition() const;


	void UpdatePosition(float deltaTime);
	void UpdatePredictedPosition(float deltaTime);
	void Translate(const sf::Vector2f& position);

	bool hasOwnership() const;
	bool SyncWithServer() const;

protected:
	ServerConnection* m_connection;
	World* m_world;

	unsigned int m_worldID;
	unsigned int m_ownership;
	bool m_serverSync{ true };

	float m_health{ 1.0f };
	float m_maxHealth{ 1.0f };
private:
    std::shared_ptr<sf::Sprite> m_sprite;
	float m_movementSpeed{ 0.0f };
	bool m_active{ true };
	std::shared_ptr<Collider> m_collider;

	//Client positions
    sf::Vector2f m_position;
	sf::Vector2f m_lastPosition;
	sf::Vector2f m_velocity{ 0.0f,0.0f };

	//Server positions
	sf::Vector2f m_networkPosition;
	sf::Vector2f m_lastNetworkPosition;
	sf::Vector2f m_networkVelocity{ 0.0f,0.0f };

};
