#pragma once
#include <SFML/System.hpp>

namespace Math {
	static float Dot(const sf::Vector2f& v1, const sf::Vector2f& v2)
	{
		return v1.x * v2.x + v1.y * v2.y;
	}

	static float SqrMagnitude(const sf::Vector2f& v)
	{
		return v.x * v.x + v.y * v.y;
	}

	static float Magnitude(const sf::Vector2f& v)
	{
		return std::sqrt(v.x * v.x + v.y * v.y);
	}


	static float Distance(const sf::Vector2f& v1, const sf::Vector2f& v2)
	{
		sf::Vector2f d(v2.x - v1.x, v2.y - v1.y);
		return Magnitude(d);
	}

	static float SqrDistance(const sf::Vector2f& v1, const sf::Vector2f& v2)
	{
		sf::Vector2f d(v2.x - v1.x, v2.y - v1.y);
		return SqrMagnitude(d);
	}

	static sf::Vector2f Normalise(const sf::Vector2f& v)
	{
		return v / Magnitude(v);
	}

	static sf::Vector2f Direction(const sf::Vector2f& v1, const sf::Vector2f& v2)
	{
		sf::Vector2f d(v2.x - v1.x, v2.y - v1.y);
		return Normalise(d);
	}

	static float Lerp(float f1, float f2, float t)
	{
		return f1 * (1 - t) + f2 * t;
	}

	static sf::Vector2f Lerp(sf::Vector2f v1, sf::Vector2f v2, float t)
	{
		float X = Lerp(v1.x, v2.x, t);
		float Y = Lerp(v1.y, v2.y, t);
		return sf::Vector2f{ X,Y };
	}

	static sf::Vector2f MoveTowards(sf::Vector2f current, sf::Vector2f target, float maxDistanceDelta)
	{
		sf::Vector2f a = target - current;
		float magnitude = Magnitude(a);
		if (magnitude <= maxDistanceDelta || magnitude == 0.0f)
		{
			return target;
		}
		return current + a / magnitude * maxDistanceDelta;
	}
}