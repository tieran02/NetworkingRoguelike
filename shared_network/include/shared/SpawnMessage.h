#pragma once
#include "Message.h"
#include "SFML/Graphics.hpp"

class SpawnMessage : public Message
{
public:
	SpawnMessage() = default;;
    SpawnMessage(char* buffer);
    SpawnMessage(unsigned int worldID, unsigned int entityID, sf::Vector2f position, unsigned int ownerID);
	unsigned int GetWorldID() const;
	unsigned int GetEntityID() const;
    sf::Vector2f GetPosition() const;
	unsigned int GetOwnershipID() const;
};
