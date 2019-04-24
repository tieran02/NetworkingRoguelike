#pragma once
#include "QuadTree.h"
#include <unordered_set>
#include <SFML/Graphics.hpp>

class CollisionManager
{
public:
	CollisionManager(const sf::FloatRect& boundsOfMap);
	~CollisionManager();

	void Add(std::shared_ptr<Collider> collider);
	void Remove(std::shared_ptr<Collider> collider);
	void Update();
	void Draw(sf::RenderWindow& window);
private:
	QuadTree m_quadTree;
	std::unordered_set<std::shared_ptr<Collider>> m_colliders;
};

