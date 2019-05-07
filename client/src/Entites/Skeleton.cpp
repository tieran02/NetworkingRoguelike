#include "Skeleton.h"
#include "Graphics/ResourceManager.h"

Skeleton::Skeleton() : Entity("Skeleton")
{
	//collide with everything except player projectiles
	GetCollider()->SetCollideMask(Collider::AllLayers() & ~(CollisionLayer::PROJECTILE_ENEMY));
}


Skeleton::~Skeleton()
{
}

void Skeleton::Start()
{
	healthText = sf::Text("Health = 100", ResourceManager::Instance().GetFont("arial"), 16);
	sf::FloatRect healthRect = healthText.getLocalBounds();
	healthText.setOrigin(healthRect.left + healthRect.width / 2.0f, (healthRect.top + healthRect.height / 2.0f) + 38.0f);
}

void Skeleton::Update(float deltaTime)
{
	UpdatePosition(deltaTime);

	healthText.setString("Health = " + std::to_string((int)m_health));
	healthText.setPosition(GetPosition());
}

void Skeleton::Draw(sf::RenderWindow & window)
{
	Entity::Draw(window);
	window.draw(healthText);
}

void Skeleton::OnCollision(Collider& other)
{
}

void Skeleton::OnLayerOverride(CollisionLayer layer)
{
}

std::shared_ptr<Entity> Skeleton::Clone(unsigned worldID, unsigned ownership, ServerConnection* connection, World* world)
{
	auto skeleton = std::make_shared<Skeleton>();
	skeleton->m_worldID = worldID;
	skeleton->m_ownership = ownership;
	skeleton->m_connection = connection;
	skeleton->m_world = world;
	return skeleton;
}
