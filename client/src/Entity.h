#pragma once
#include <SFML/Graphics.hpp>
#include <memory>
#include "shared/Collider.h"

class World;
class ServerConnection;
class Entity
{
public:
    Entity(const std::string& spriteName);
    virtual ~Entity();

    virtual void Start() = 0;
    virtual void Update(float deltaTime) = 0;
    virtual void Draw(sf::RenderWindow & window) = 0;
    virtual std::shared_ptr<Entity> Clone(unsigned int worldID, unsigned int ownership, ServerConnection* connection, World* world) = 0;

	unsigned int GetWorldID() const { return m_worldID; }
	const std::shared_ptr<sf::Sprite>& GetSprite() const { return m_sprite; }
    void SetPosition(const sf::Vector2f& position);
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
	std::shared_ptr<Collider>& GetCollider() { return m_collider; }


	sf::Vector2f CalculatePredictedPosition() const;


	void UpdatePosition(float deltaTime);
	void UpdatePredictedPosition(float deltaTime);
	void Translate(const sf::Vector2f& position);

	bool hasOwnership() const;

protected:
	ServerConnection* m_connection;
	World* m_world;

	unsigned int m_worldID;
	unsigned int m_ownership;
    std::shared_ptr<sf::Sprite> m_sprite;
    sf::Vector2f m_position;
	sf::Vector2f m_lastPosition;
	sf::Vector2f m_networkPosition;
	sf::Vector2f m_lastNetworkPosition;

	float m_movementSpeed{0.0f};

	sf::Vector2f m_velocity{0.0f,0.0f};
	sf::Vector2f m_networkVelocity{ 0.0f,0.0f };

	std::shared_ptr<Collider> m_collider;
};
