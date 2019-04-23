#pragma once
#include <SFML/Graphics.hpp>

class Entity;

enum CollisionLayer
{
	NONE = 0,
	PLAYER = 1,
	ENEMY = 2,
	PROJECTILE_ENEMY = 4,
	PROJECTILE_PLAYER = 8,
	WALL = 16,
};

class Collider
{
public:
	Collider(const sf::Vector2f& pos, const sf::Vector2f& size, CollisionLayer layer);
	Collider(const sf::RectangleShape& rect, CollisionLayer layer);
	~Collider();

	void Move(float x, float y);

	bool CheckCollision(Collider& other);
	sf::Vector2f GetPosition() const;
	void SetPosition(const sf::Vector2f& pos);
	sf::Vector2f GetHalfSize() const;
	const sf::RectangleShape& GetRect() const { return m_rect; }
	void SetMoveable(bool moveable);

	void SetCollideMask(unsigned int collideWithMask);
	void SetLayer(CollisionLayer layer);
	static unsigned int AllLayers();
private:
	sf::RectangleShape m_rect;
	sf::Vector2f m_halfSize;
	bool m_moveable{ false };
	unsigned int m_layer;
	unsigned int m_collideWith;
};

