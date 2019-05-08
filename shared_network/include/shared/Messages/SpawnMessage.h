#pragma once
#include "Message.h"
#include "SFML/Graphics.hpp"
#include "../Collider.h"

class SpawnMessage : public Message
{
public:
	//Size of message includng header
	const static size_t SIZE;

	SpawnMessage() = default;;
    SpawnMessage(char* buffer);
    SpawnMessage(unsigned int worldID, unsigned int entityID, sf::Vector2f position, sf::Vector2f velocity, unsigned int ownerID, CollisionLayer layerOverride);
	unsigned int GetWorldID() const;
	unsigned int GetEntityID() const;
    sf::Vector2f GetPosition() const;
	sf::Vector2f GetVelocity() const;
	unsigned int GetOwnershipID() const;
	CollisionLayer GetLayerOverride() const;
};
