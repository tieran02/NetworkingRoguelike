#pragma once
#include <SFML/System.hpp>

namespace Math {
	static float Dot(const sf::Vector2f& v1, const sf::Vector2f& v2)
	{
		return v1.x * v2.x + v1.y * v2.y;
	}

	static float Length(const sf::Vector2f& v)
	{
		return std::sqrt(v.x * v.x + v.y * v.y);
	}

	static float Distance(const sf::Vector2f& v1, const sf::Vector2f& v2)
	{
		sf::Vector2f d(v2.x - v1.x, v2.y - v1.y);
		return Length(d);
	}
}