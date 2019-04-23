#include "Player.h"
#include "Networking/ServerConnection.h"
#include "World.h"
#include "shared/Utility/Math.h"

Player::Player() : Entity("Player", CollisionLayer::PLAYER)
{
	SetMaxHealth(100.0f);
	SetHealth(GetMaxHealth());
	SetMovementSpeed(200.0f);
	//collide with everything except player projectiles
	GetCollider()->SetCollideMask(Collider::AllLayers() & ~(CollisionLayer::PROJECTILE_PLAYER));
}

Player::~Player()
{

}

void Player::Start()
{
	if (hasOwnership())
	{
		GetCollider()->SetMoveable(true);
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
			newVelocity += sf::Vector2f(-GetMovementSpeed(), 0.0f);
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
		{
			// left key is pressed: move our character
			newVelocity += sf::Vector2f(GetMovementSpeed(), 0.0f);
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
		{
			// left key is pressed: move our character
			newVelocity += sf::Vector2f(0.0f, -GetMovementSpeed());
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
		{
			// left key is pressed: move our character
			newVelocity += sf::Vector2f(0.0f, GetMovementSpeed());
		}

		if(sf::Keyboard::isKeyPressed(sf::Keyboard::Space))
		{
			m_world->ShootBullet(GetPosition(), sf::Vector2f(0.0f, -400.0f));
		}
		SetVelocity(newVelocity);
		UpdatePosition(deltaTime);

		//if health is lower than zero disable the player
		if(GetHealth() <= 0.0f)
		{
			SetActive(false);
		}

		//set camera pos
		m_world->GetCamera().SetPosition(GetPosition());
	}else
	{
		UpdatePredictedPosition(deltaTime); //network player
	}
}

void Player::Draw(sf::RenderWindow & window)
{
    window.draw(*GetSprite());
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
