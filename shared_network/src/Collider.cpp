#include "Collider.h"
#include "Utility/Math.h"


Collider::Collider(const sf::Vector2f& pos, const sf::Vector2f& size)
{
	m_rect = sf::RectangleShape{ size };
	m_rect.setPosition(pos);
	m_rect.setFillColor(sf::Color::Green);
	m_halfSize = size / 2.0f;
}

Collider::Collider(const sf::RectangleShape& rect) : m_rect(rect)
{
	m_halfSize = m_rect.getSize() / 2.0f;
	m_rect.setFillColor(sf::Color::Green);

}


Collider::~Collider()
{
}

void Collider::Move(float x, float y)
{
	m_rect.move( sf::Vector2f{ x,y });
}

bool Collider::CheckCollision(Collider& other)
{
	sf::Vector2f otherPos = other.GetPosition();
	sf::Vector2f otherHalfSize = other.GetHalfSize();
	sf::Vector2f thisPos = GetPosition();
	sf::Vector2f thisHalfSize = GetHalfSize();

	sf::Vector2f d{ otherPos.x - thisPos.x ,otherPos.y - thisPos.y };

	float intersectX = std::fabs(d.x) - (otherHalfSize.x + thisHalfSize.x);
	float intersectY = std::fabs(d.y) - (otherHalfSize.y + thisHalfSize.y);
	float push = 1.0f;
	if (m_moveable)
		push = 0.0f;

	if(intersectX < 0.0f && intersectY < 0.0f)
	{

		if(intersectX > intersectY)
		{
			if(d.x > 0.0f)
			{
				Move(intersectX * (1.0f - push), 0.0f);
				other.Move(-intersectX * push, 0.0f);
			}
			else
			{
				Move(-intersectX * (1.0f - push), 0.0f);
				other.Move(intersectX, 0.0f);
			}
		}
		else
		{
			if (d.y > 0.0f)
			{
				Move(0.0f, intersectX * (1.0f - push));
				other.Move(0.0f, -intersectX * push);
			}
			else
			{
				Move(0.0f, -intersectX * (1.0f - push));
				other.Move(0.0f, intersectX * push);
			}
		}

		return true;
	}

	return false;
}

sf::Vector2f Collider::GetPosition() const
{
	return m_rect.getPosition();
}

void Collider::SetPosition(const sf::Vector2f& pos)
{
	m_rect.setPosition(pos);
}

sf::Vector2f Collider::GetHalfSize() const
{
	return m_halfSize;
}

void Collider::SetMoveable(bool moveable)
{
	m_moveable = moveable;
}
