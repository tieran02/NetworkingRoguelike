#include "Collider.h"
#include "Utility/Math.h"
#include "Utility/Log.h"


Collider::Collider(const sf::Vector2f& pos, const sf::Vector2f& size, Entity* entity, CollisionLayer layer) : qt(nullptr)
{
	m_rect = sf::RectangleShape{ size };
	m_rect.setPosition(pos);
	m_rect.setFillColor(sf::Color{ 0,255,0,200 });
	m_halfSize = size / 2.0f;
	m_rect.setOrigin(m_halfSize);

	m_entity = entity;
	m_layer = layer;
	m_collideWith = AllLayers();
}

Collider::Collider(const sf::RectangleShape& rect, CollisionLayer layer) : m_rect(rect)
{
	m_halfSize = m_rect.getSize() / 2.0f;
	m_rect.setFillColor(sf::Color{ 0,255,0,200 });
	m_rect.setOrigin(m_halfSize);

	m_layer = layer;
	m_collideWith = AllLayers();
}


Collider::~Collider()
{
}

void Collider::Move(float x, float y)
{
	m_rect.move(sf::Vector2f{ x,y });
}

bool Collider::CheckCollision(Collider& other)
{
	if (!m_active || !other.m_active)
		return false;

	//check the layers
	const bool bit = (m_collideWith & other.m_layer) != 0;
	const bool bit1 = (other.m_collideWith & m_layer) != 0;
	if (!bit || !bit1)
		return false;



	sf::Vector2f otherPos = other.GetPosition();
	sf::Vector2f otherHalfSize = other.GetHalfSize();
	sf::Vector2f thisPos = GetPosition();
	sf::Vector2f thisHalfSize = GetHalfSize();

	sf::Vector2f d{ otherPos.x - thisPos.x ,otherPos.y - thisPos.y };

	const float intersectX = std::abs(d.x) - (otherHalfSize.x + thisHalfSize.x);
	const float intersectY = std::abs(d.y) - (otherHalfSize.y + thisHalfSize.y);

	float push = 1.0f;
	if (m_moveable)
	{
		push = 0.0f;
	}

	if (intersectX < 0.0f && intersectY < 0.0f)
	{

		if (intersectX > intersectY)
		{
			if (d.x > 0.0f)
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

sf::FloatRect Collider::GetRect() const
{
	return m_rect.getGlobalBounds();
}

void Collider::SetMoveable(bool moveable)
{
	m_moveable = moveable;
}

void Collider::SetCollideMask(unsigned collideWithMask)
{
	m_collideWith = collideWithMask;
}

void Collider::SetLayer(CollisionLayer layer)
{
	m_layer = layer;
}

CollisionLayer Collider::GetLayer() const
{
	return m_layer;
}

unsigned int Collider::AllLayers()
{
	return std::numeric_limits<unsigned int>::max();
}

Entity* Collider::GetEntity() const
{
	return m_entity;
}

void Collider::SetActive(bool active)
{
	m_active = active;
}

bool Collider::IsActive() const
{
	return  m_active;
}

