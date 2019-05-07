#pragma once
#include <string>
#include "World.h"

enum class AreaOfAttack
{
	SINGLE,
	CONE,
	CIRCLE
};

class Weapon
{
public:
	Weapon(const std::string& bulletEntity, AreaOfAttack areaOfAttack, CollisionLayer side, float speed, float cooldown = 100.0f, int bulletCount = 1, float delay = 100.0f);
	~Weapon();

	void Fire(World& world, sf::Vector2f position, sf::Vector2f direction);
private:
	std::string m_entityName;
	CollisionLayer m_side;
	AreaOfAttack m_areaOfAttack;
	int m_bulletCount;
	float m_speed;
	float m_deleyPerShot;
	float m_coolDown;

	double lastFire{ 0.0f };

	void singleShot(World& world, sf::Vector2f position, sf::Vector2f direction);
	void coneShot(World& world, sf::Vector2f position, sf::Vector2f direction);
	void circleShot(World& world, sf::Vector2f position);
};

