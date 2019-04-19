#pragma once
#include <SFML/Graphics.hpp>
#include <memory>

class World;
class ServerConnection;
class Entity
{
public:
    Entity();
    virtual ~Entity();

    virtual void Start() = 0;
    virtual void Update() = 0;
    virtual void Draw(sf::RenderWindow & window) = 0;
    virtual std::shared_ptr<Entity> Clone(unsigned int worldID, unsigned int ownership, ServerConnection* connection, World* world) = 0;

	unsigned int GetWorldID() const { return m_worldID; }
    void SetPosition(const sf::Vector2f& position);
	sf::Vector2f GetPosition() const { return m_position; }
	void SetNetworkPosition(const sf::Vector2f& position);
	sf::Vector2f GetNetworkPosition() const { return m_networkPosition; }
	sf::Vector2f GetDirection() const;
	void SetVelocity(sf::Vector2f velocity);
	sf::Vector2f GetVelocity() const;
	void SetNetworkVelocity(const sf::Vector2f& velocity);
	sf::Vector2f GetNetworkVelocity() const { return m_networkVelocity; }


	void UpdatePosition();
	void Translate(const sf::Vector2f& position);


protected:
	unsigned int m_worldID;
	ServerConnection* m_connection;
	World* m_world;
	unsigned int m_ownership;
    sf::Sprite m_sprite;
    sf::Vector2f m_position;
	sf::Vector2f m_lastPosition;
	sf::Vector2f m_networkPosition;
	sf::Vector2f m_velocity{0.0f,0.0f};
	sf::Vector2f m_networkVelocity{ 0.0f,0.0f };
	bool m_movemnetUpdate{ false };

	bool hasOwnership() const;
};
