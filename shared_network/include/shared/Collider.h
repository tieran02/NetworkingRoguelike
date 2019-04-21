#pragma once
#include <SFML/Graphics.hpp>

class Entity;
class Collider
{
public:
	Collider(const sf::Vector2f& pos, const sf::Vector2f& size);
	Collider(const sf::RectangleShape& rect);
	~Collider();

	void Move(float x, float y);

	bool CheckCollision(Collider& other);
	sf::Vector2f GetPosition() const;
	void SetPosition(const sf::Vector2f& pos);
	sf::Vector2f GetHalfSize() const;
	const sf::RectangleShape& GetRect() const { return m_rect; }
	void SetMoveable(bool moveable);
private:
	sf::RectangleShape m_rect;
	sf::Vector2f m_halfSize;
	bool m_moveable{ false };
};

