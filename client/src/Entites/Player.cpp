#include "Player.h"
#include "Networking/ServerConnection.h"
#include "World.h"
#include "shared/Utility/Math.h"

Player::Player() : Entity("Player")
{
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
	if (hasOwnership() && m_world->IsWindowFocused())
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

		if (sf::Mouse::isButtonPressed(sf::Mouse::Left) && m_fireTimeCounter >= .25f)
		{
			const sf::Vector2i mousePos = sf::Mouse::getPosition(m_world->GetWindow());
			const sf::Vector2f mouseWorldPos = Camera::ScreenToWorldPos(mousePos, m_world->GetWindow());
			const sf::Vector2f dir = Math::Direction(GetPosition(), mouseWorldPos);

			m_world->ShootBullet(GetPosition(), dir * 400.0f, CollisionLayer::PROJECTILE_PLAYER);
			m_fireTimeCounter = 0.0f;
		}
		m_fireTimeCounter += deltaTime;
		SetVelocity(newVelocity);
		UpdatePosition(deltaTime);

		//set camera pos
		m_world->GetCamera().SetPosition(GetPosition());
	}
	else
	{
		UpdatePredictedPosition(deltaTime); //network player
	}
}

void Player::OnCollision(Collider& other)
{
	//Damage(5.0f);
}

void Player::OnLayerOverride(CollisionLayer layer)
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
