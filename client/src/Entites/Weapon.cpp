#include "Weapon.h"
#include <thread>
#include <chrono>
#include "shared/Random.h"

Weapon::Weapon(const std::string& bulletEntity, AreaOfAttack areaOfAttack, CollisionLayer side, float speed, float cooldown, int bulletCount, float delay) :
	m_entityName(bulletEntity),
	m_areaOfAttack(areaOfAttack),
	m_side(side),
	m_speed(speed),
	m_bulletCount(bulletCount),
	m_deleyPerShot(delay),
	m_coolDown(cooldown)
{
}

Weapon::~Weapon()
{
}

Weapon Weapon::CreateWeapon(AreaOfAttack areaOfAttack)
{
	float speed = 200.0f;
	float cooldown = 100.0f;
	int bulletCount = 1;
	switch (areaOfAttack)
	{
	case AreaOfAttack::SINGLE:
		speed = (float)Random::randInt(250, 500);
		cooldown = (float)Random::randInt(100, 500);
		break;
	case AreaOfAttack::CONE:
		speed = (float)Random::randInt(250, 400);
		cooldown = (float)Random::randInt(400, 1000);
		bulletCount = Random::randInt(3, 9);

		break;
	case AreaOfAttack::CIRCLE:
		speed = (float)Random::randInt(225, 350);
		cooldown = (float)Random::randInt(400, 1500);
		bulletCount = Random::randInt(6, 16);
		break;
	default:
		break;
	}
	return Weapon{ "Bullet",areaOfAttack,CollisionLayer::PLAYER,speed, cooldown,bulletCount };
}

void Weapon::Fire(World& world, sf::Vector2f position, sf::Vector2f direction)
{
	float time = std::chrono::duration<float, std::ratio<1, 1000>>(std::chrono::steady_clock::now().time_since_epoch()).count();
	if (time >= lastFire + m_coolDown)
	{
		switch (m_areaOfAttack)
		{
		case AreaOfAttack::SINGLE:
			singleShot(world, position, direction);
			break;
		case AreaOfAttack::CONE:
			coneShot(world, position, direction);
			break;
		case AreaOfAttack::CIRCLE:
			circleShot(world, position);
			break;
		default:
			break;
		}

		lastFire = time;
	}
}

void Weapon::AddFirerate(float rate)
{
	m_coolDown = std::max(m_coolDown - rate, 100.0f);
}

void Weapon::singleShot(World& world, sf::Vector2f position, sf::Vector2f direction)
{
	world.ShootBullet(position, direction * m_speed, m_side);
}

void Weapon::coneShot(World & world, sf::Vector2f position, sf::Vector2f direction)
{
	constexpr float MAX_ANGLE = 15.0;

	//center 
	world.ShootBullet(position, direction * m_speed, m_side);

	int halfBulletCount = std::max(1, (m_bulletCount - 1) / 2);
	const float ANGLE_OFFSET = MAX_ANGLE / (float)halfBulletCount;

	//left side
	for (int i = 0; i < halfBulletCount; i++)
	{
		sf::Vector2f dir = Math::Rotate(direction, -ANGLE_OFFSET * (i+1));
		world.ShootBullet(position, dir * m_speed, m_side);

	}

	//right side
	for (int i = 0; i < halfBulletCount; i++)
	{
		sf::Vector2f dir = Math::Rotate(direction, ANGLE_OFFSET * (i+1));
		world.ShootBullet(position, dir * m_speed, m_side);
	}


}

void Weapon::circleShot(World & world, sf::Vector2f position)
{
	const float ANGLE_OFFSET = 360.0f / m_bulletCount;
	sf::Vector2f upVector{ 0.0f,1.0f };
	for (int i = 0; i < m_bulletCount; i++)
	{
		sf::Vector2f dir = Math::Rotate(upVector, ANGLE_OFFSET * (i + 1));
		world.ShootBullet(position, dir * m_speed, m_side);
	}
}
