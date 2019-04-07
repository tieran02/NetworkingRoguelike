#pragma once
#include <SFML/Graphics.hpp>
#include <memory>

class ServerConnection;
class Entity
{
public:
    Entity();
    virtual ~Entity();

    virtual void Start() = 0;
    virtual void Update() = 0;
    virtual void Draw(sf::RenderWindow & window) = 0;
    virtual std::shared_ptr<Entity> Clone(unsigned int worldID, unsigned int ownership, ServerConnection* connection) = 0;

    void SetPosition(const sf::Vector2f& position);
	sf::Vector2f GetPosition() const { return m_position; }
	void SetNetworkPosition(const sf::Vector2f& position);
	sf::Vector2f GetNetworkPosition() const { return m_networkPosition; }

	void Translate(const sf::Vector2f& position);

protected:
	unsigned int m_worldID;
	ServerConnection* m_connection;
	unsigned int m_ownership;
    sf::Sprite m_sprite;
    sf::Vector2f m_position;
	sf::Vector2f m_networkPosition;

	bool hasOwnership() const;
};
