#pragma once
#include "Message.h"
#include "SFML/Graphics.hpp"


//Entity state is used to update the complete entity within the world (ONLY SEND FROM TCP)
class EntityStateMessage : public Message
{
public:
	//Size of message includng header
	const static size_t SIZE;

	EntityStateMessage() = default;;
	EntityStateMessage(char* buffer);
	EntityStateMessage(unsigned int worldID, sf::Vector2f position, sf::Vector2f velocity, bool active, bool destroy, unsigned int senderID);
	unsigned int WorldID() const;
	sf::Vector2f GetPosition() const;
	sf::Vector2f GetVelocity() const;
	bool IsActive() const;
	bool ShouldDestroy() const;
};
