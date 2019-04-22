#include "Player.h"
#include "Networking/ServerConnection.h"
#include "World.h"
#include "shared/Utility/Math.h"

Player::Player() : Entity("Player")
{
	m_movementSpeed = 200.0f;
}

Player::~Player()
{

}

void Player::Start()
{
	if (hasOwnership())
	{
		m_collider->SetMoveable(true);
	}
}

void Player::Update(float deltaTime)
{
	if(hasOwnership() && m_world->IsWindowFocused())
	{
		sf::Vector2f newVelocity;

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
		{
			// left key is pressed: move our character
			newVelocity += sf::Vector2f(-m_movementSpeed, 0.0f);
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
		{
			// left key is pressed: move our character
			newVelocity += sf::Vector2f(m_movementSpeed, 0.0f);
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
		{
			// left key is pressed: move our character
			newVelocity += sf::Vector2f(0.0f, -m_movementSpeed);
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
		{
			// left key is pressed: move our character
			newVelocity += sf::Vector2f(0.0f, m_movementSpeed);
		}

		if(sf::Keyboard::isKeyPressed(sf::Keyboard::Space))
		{
			m_world->ShootBullet(m_position, sf::Vector2f(0.0f, -400.0f));
		}
		SetVelocity(newVelocity);
		UpdatePosition(deltaTime);

		//set camera pos
		m_world->GetCamera().SetPosition(m_position);
	}else
	{
		UpdatePredictedPosition(deltaTime); //network player
	}
}

void Player::Draw(sf::RenderWindow & window)
{
    window.draw(*m_sprite);
}

void Player::OnCollision(Collider& other)
{
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
