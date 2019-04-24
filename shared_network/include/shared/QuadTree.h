#pragma once
#include <vector>
#include <SFML/Graphics/Rect.hpp>
#include <array>


class Collider;

class QuadTree {

	enum Quadrant
	{
		NotFound = -1,
		TopLeft,
		TopRight,
		BottomLeft,
		BottomRight,
	};

public:
	QuadTree(std::size_t Level, const sf::FloatRect& Bounds);
	~QuadTree();

	void clear();
	void insert(Collider* object);
	void getCloseObjects(const sf::FloatRect& Bounds, std::vector<Collider*>& returnObjects);
private:
	std::vector<Collider*> m_objects;
	std::array< std::unique_ptr<QuadTree>, 4> m_children;

	void split();
	Quadrant getIndex(const sf::FloatRect& Rect) const;
	bool insertInChild(Collider* object) const;
	bool hasChildren() const;

	sf::FloatRect m_bounds;
	sf::FloatRect m_globalBounds;
	std::size_t m_level;
};
