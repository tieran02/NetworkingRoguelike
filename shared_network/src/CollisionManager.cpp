#include "CollisionManager.h"
#include <memory>
#include "Collider.h"

CollisionManager::CollisionManager(const sf::FloatRect& boundsOfMap) : m_quadTree(1, boundsOfMap)
{
}

CollisionManager::~CollisionManager()
{
}

void CollisionManager::Add(std::shared_ptr<Collider> collider)
{
	m_colliders.insert(collider);
}

void CollisionManager::Remove(std::shared_ptr<Collider> collider)
{
	m_colliders.erase(collider);
}

void CollisionManager::Update()
{
	m_quadTree.clear();
	for (auto collider : m_colliders)
	{
		m_quadTree.insert(collider.get());
	}

	std::vector<Collider*> returnedItems;
	for (auto collider : m_colliders)
	{
		returnedItems.clear();
		m_quadTree.getCloseObjects(collider->GetRect(), returnedItems);

		for (auto otherCollider : returnedItems)
		{
			if(collider.get() == otherCollider)
				continue;
			collider->CheckCollision(*otherCollider);
		}
	}
}

void CollisionManager::Draw(sf::RenderWindow& window)
{
	for (auto& collider : m_colliders)
	{
		if (collider->IsActive())
			window.draw(collider->GetRectShape());
	}
}
