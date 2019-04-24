#include "QuadTree.h"
#include "Collider.h"
#include <functional>
#include <cassert>

constexpr auto  MaxLevels = 5u;
constexpr auto  MaxObjects = 2000u;

QuadTree::QuadTree(std::size_t Level, const sf::FloatRect& Bounds)
	: m_objects()
	, m_children()
	, m_bounds(Bounds)
	, m_level(Level)
{
	if(m_level == 1)
	{
		m_globalBounds = Bounds;
	}
}

QuadTree::~QuadTree()
{
	clear();
}

void QuadTree::clear()
{
	m_objects.clear();

	if (!hasChildren())
		return;

	for (auto& child : m_children)
	{
		child->clear();
		child.reset();
	}
}

void QuadTree::insert(Collider* object)
{
	if (hasChildren() && insertInChild(object))
		return;

	if(!m_globalBounds.contains(object->GetRectShape().getPoint(0)))
		return;

	m_objects.push_back(object);

	// This node is already split, and we can't move any objects down.
	if (hasChildren())
		return;

	// Can't split this node, so no point checking number of objects.
	if (m_level == MaxLevels)
		return;

	// Don't need to split this node yet.
	if (m_objects.size() < MaxObjects)
		return;

	split();

	m_objects.erase(
		std::remove_if(m_objects.begin(), m_objects.end(),
			std::bind(&QuadTree::insertInChild, this, std::placeholders::_1)),
		m_objects.end());
}

void QuadTree::getCloseObjects(const sf::FloatRect& Bounds, std::vector<Collider*>& returnObjects)
{
	if (hasChildren())
	{
		auto index = getIndex(Bounds);

		if (index != NotFound)
			m_children[index]->getCloseObjects(Bounds, returnObjects);
	}

	//std::copy(m_objects.begin(), m_objects.end(), std::back_inserter(returnObjects));
	returnObjects.insert(std::end(returnObjects), std::begin(m_objects), std::end(m_objects));
}

void QuadTree::split()
{
	auto width = m_bounds.width / 2.f;
	auto height = m_bounds.height / 2.f;
	auto x = m_bounds.left;
	auto y = m_bounds.top;

	m_children[TopLeft] = std::make_unique<QuadTree>(m_level + 1, sf::FloatRect(x, y, width, height));
	m_children[TopRight] = std::make_unique<QuadTree>(m_level + 1, sf::FloatRect(x + width, y, width, height));
	m_children[BottomLeft] = std::make_unique<QuadTree>(m_level + 1, sf::FloatRect(x, y + height, width, height));
	m_children[BottomRight] = std::make_unique<QuadTree>(m_level + 1, sf::FloatRect(x + width, y + height, width, height));
}

QuadTree::Quadrant QuadTree::getIndex(const sf::FloatRect& bounds) const
{
	assert(bounds.height > 0.f);
	assert(bounds.width > 0.f);

	auto verticalMidpoint = m_bounds.left + m_bounds.width / 2.f;
	auto horizontalMidpoint = m_bounds.top + m_bounds.height / 2.f;

	// Can the object "completely" fit within this quadrant?
	bool top = (bounds.top + bounds.height < horizontalMidpoint);
	bool bottom = (bounds.top > horizontalMidpoint);
	bool left = (bounds.left + bounds.width < verticalMidpoint);
	bool right = (bounds.left > verticalMidpoint);

	if (top && left)
		return TopLeft;

	if (top && right)
		return TopRight;

	if (bottom && left)
		return BottomLeft;

	if (bottom && right)
		return BottomRight;

	return NotFound;
}

bool QuadTree::insertInChild(Collider* object) const
{
	assert(hasChildren());

	const Quadrant index = getIndex(object->GetRect());

	if (index == NotFound)
		return false;

	m_children[index]->insert(object);

	return true;
}

bool QuadTree::hasChildren() const
{
	return(m_children[0] != nullptr);
}
