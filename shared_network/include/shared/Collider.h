#pragma once
#include <SFML/Graphics.hpp>
#include "QuadTree.h"

class Entity;

enum CollisionLayer
{
	NONE = 0,
	PLAYER = 1,
	ENEMY = 2,
	PROJECTILE_ENEMY = 4,
	PROJECTILE_PLAYER = 8,
	PROJECTILE_NEUTRAL = 16,
	WALL = 32,
};

class Collider
{
	friend QuadTree;
public:
	Collider(const sf::Vector2f& pos, const sf::Vector2f& size, Entity* entity, CollisionLayer layer);
	Collider(const sf::RectangleShape& rect, CollisionLayer layer);
	~Collider();

	void Move(float x, float y);

	bool CheckCollision(Collider& other);
	sf::Vector2f GetPosition() const;
	void SetPosition(const sf::Vector2f& pos);
	sf::Vector2f GetHalfSize() const;
	const sf::RectangleShape& GetRectShape() const { return m_rect; }
	sf::FloatRect GetRect() const;

	void SetMoveable(bool moveable);

	void SetCollideMask(unsigned int collideWithMask);
	void SetLayer(CollisionLayer layer);
	CollisionLayer GetLayer() const;
	static unsigned int AllLayers();

	Entity* GetEntity() const;

	void SetActive(bool active);
	bool IsActive() const;
private:
	Entity* m_entity{ nullptr };
	sf::RectangleShape m_rect;
	sf::Vector2f m_halfSize;
	bool m_moveable{ false };
	CollisionLayer m_layer;
	unsigned int m_collideWith;
	bool m_active{ true };
	QuadTree* qt = nullptr;
};

