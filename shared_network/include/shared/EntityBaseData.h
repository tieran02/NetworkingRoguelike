#pragma once
#include <string>
#include "Collider.h"

struct EntityBaseData
{
	unsigned int EntityID;
	std::string EntityName;
	std::string EntitySpriteName;
	unsigned int SpriteWidth;
	unsigned int SpriteHeight;
	float Health;
	float MaxHealth;
	CollisionLayer Layer;
};
