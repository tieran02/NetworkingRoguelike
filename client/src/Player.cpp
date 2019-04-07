#include "Player.h"
#include "Networking/ServerConnection.h"

Player::Player()
{

}

Player::~Player()
{

}

void Player::Start()
{

}

void Player::Update()
{
	if(hasOwnership())
	{
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
		{
			// left key is pressed: move our character
			Translate(sf::Vector2f(1.0f, 0.0f));
			m_connection->SendMovementMessage(m_worldID, m_position);
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
		{
			// left key is pressed: move our character
			Translate(sf::Vector2f(-1.0f, 0.0f));
			m_connection->SendMovementMessage(m_worldID, m_position);
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
		{
			// left key is pressed: move our character
			Translate(sf::Vector2f(0.0f, 1.0f));
			m_connection->SendMovementMessage(m_worldID, m_position);
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
		{
			// left key is pressed: move our character
			Translate(sf::Vector2f(0.0f, -1.0f));
			m_connection->SendMovementMessage(m_worldID, m_position);
		}
	}
}

void Player::Draw(sf::RenderWindow & window)
{
    sf::CircleShape shape(10);
    shape.setFillColor(sf::Color(100, 250, 50));
    shape.setPosition(m_position);
    window.draw(shape);
}

std::shared_ptr<Entity> Player::Clone(unsigned int worldID, unsigned int ownership, ServerConnection* connection)
{
	auto player = std::make_shared<Player>();
	player->m_worldID = worldID;
	player->m_ownership = ownership;
	player->m_connection = connection;
	return player;
}
