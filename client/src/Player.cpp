#include "Player.h"
#include "Networking/ServerConnection.h"
#include "World.h"
#include "shared/Utility/Math.h"

Player::Player()
{
	m_movementSpeed = 100.0f;
}

Player::~Player()
{

}

void Player::Start()
{

}

void Player::Update(float deltaTime)
{
	if(hasOwnership() && m_world->IsWindowFocused())
	{
		sf::Vector2f newVelocity;

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
		{
			// left key is pressed: move our character
			newVelocity += sf::Vector2f(m_movementSpeed * deltaTime, 0.0f);
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
		{
			// left key is pressed: move our character
			newVelocity += sf::Vector2f(-m_movementSpeed * deltaTime, 0.0f);
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
		{
			// left key is pressed: move our character
			newVelocity += sf::Vector2f(0.0f, m_movementSpeed * deltaTime);
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
		{
			// left key is pressed: move our character
			newVelocity += sf::Vector2f(0.0f, -m_movementSpeed * deltaTime);
		}
		SetVelocity(newVelocity);
		UpdatePosition(deltaTime);
	}else
	{
		UpdatePredictedPosition(deltaTime); //network player
	}
}

void Player::Draw(sf::RenderWindow & window)
{
    sf::CircleShape shape(10);
    shape.setFillColor(sf::Color(100, 250, 50));
    shape.setPosition(m_position);
    window.draw(shape);
}

std::shared_ptr<Entity> Player::Clone(unsigned int worldID, unsigned int ownership, ServerConnection* connection, World* world)
{
	auto player = std::make_shared<Player>();
	player->m_worldID = worldID;
	player->m_ownership = ownership;
	player->m_connection = connection;
	player->m_world = world;
	return player;
}