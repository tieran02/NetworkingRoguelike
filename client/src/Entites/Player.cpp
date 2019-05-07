#include "Player.h"
#include "Networking/ServerConnection.h"
#include "World.h"
#include "shared/Utility/Math.h"
#include "Graphics/ResourceManager.h"

Player::Player() : Entity("Player"), m_weapon("Bullet", AreaOfAttack::CIRCLE, CollisionLayer::PROJECTILE_PLAYER, 400.0f, 250.0f, 12, 1000.0f)
{
	//collide with everything except player projectiles
	GetCollider()->SetCollideMask(Collider::AllLayers() & ~(CollisionLayer::PROJECTILE_PLAYER));
}

Player::~Player()
{

}

void Player::Start()
{
	nameText = sf::Text(m_connection->GetPlayerNames().at(m_ownership), ResourceManager::Instance().GetFont("arial"), 24);
	sf::FloatRect textRect = nameText.getLocalBounds();
	nameText.setOrigin(textRect.left + textRect.width / 2.0f, (textRect.top + textRect.height / 2.0f) + 54.0f);

	healthText = sf::Text("Health = 100", ResourceManager::Instance().GetFont("arial"), 18);
	sf::FloatRect healthRect = healthText.getLocalBounds();
	healthText.setOrigin(healthRect.left + healthRect.width / 2.0f, (healthRect.top + healthRect.height / 2.0f) + 38.0f);

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

		if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
		{
			const sf::Vector2i mousePos = sf::Mouse::getPosition(m_world->GetWindow());
			const sf::Vector2f mouseWorldPos = Camera::ScreenToWorldPos(mousePos, m_world->GetWindow());
			const sf::Vector2f dir = Math::Direction(GetPosition(), mouseWorldPos);

			m_weapon.Fire(*m_world, GetPosition(), dir);
		}

		SetVelocity(newVelocity);
		UpdatePosition(deltaTime);

		//set camera pos
		m_world->GetCamera().SetPosition(GetPosition());
	}
	else
	{
		UpdatePosition(deltaTime); //network player
	}

	nameText.setPosition(GetPosition());

	healthText.setString("Health = " + std::to_string((int)m_health));
	healthText.setPosition(GetPosition());
}

void Player::Draw(sf::RenderWindow & window)
{
	Entity::Draw(window);
	window.draw(nameText);
	window.draw(healthText);
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
