#pragma once
#include <SFML/System.hpp>
#include <cmath>

namespace Math {
	# define PI 3.14159265358979323846  /* pi */

	struct Rect
	{
		Rect() : Left(0), Right(0), Top(0), Bottom(0) {}
		Rect(float left, float right, float top, float bottom) : Left(left), Right(right), Top(top), Bottom(bottom) {}

		float Left, Right, Top, Bottom;

		float Width() const { return (Right - Left); }
		float Height() const { return (Bottom - Top); }

		bool Intersect(const Rect& otherRect)
		{
			if (std::max(Left, otherRect.Left) <= std::min(Right, otherRect.Right) && std::max(Top, otherRect.Top) <= std::min(Bottom, otherRect.Bottom))
				return true;

			return false;
		}
		//A single point intersects
		//Rectangle COMPLETELY contains OTHER_Rect 
		bool Contains(const Rect& otherRect)
		{
			if (otherRect.Right <= Right && otherRect.Left >= Left && otherRect.Top >= Top && otherRect.Bottom <= Bottom)
				return true;
			return false;
		}
		//Check of THIS rect is outide OTHER_Rect
		bool Outside(const Rect& otherRect)
		{
			return !Intersect(otherRect);
		}



		//Add a vector to the current vector
		Rect& operator+=(const Rect& rhs)
		{
			Left += rhs.Left;
			Right += rhs.Right;
			Top += rhs.Top;
			Bottom += rhs.Bottom;
			return *this;
		}
	};

	static const float DegToRad = (float)PI / (float)180;

	static sf::Vector2f RotateRadians(const sf::Vector2f& v, float radians)
	{
		float ca = std::cos(radians);
		float sa = std::sin(radians);
		return sf::Vector2f(ca*v.x - sa * v.y, sa*v.x + ca * v.y);
	}

	static sf::Vector2f Rotate(const sf::Vector2f& v, float degrees)
	{
		return RotateRadians(v, degrees * DegToRad);
	}

	static float Clamp(float n, float lower, float upper) 
	{
		return std::max(lower, std::min(n, upper));
	}

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