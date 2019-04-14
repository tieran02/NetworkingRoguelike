#pragma once
#include "Message.h"
#include "SFML/Graphics.hpp"


//Entity state is used to update the complete entity within the world (ONLY SEND FROM TCP)
class EntityStateMessage : public Message
{
public:
	EntityStateMessage(char* buffer);
	EntityStateMessage(unsigned int worldID, sf::Vector2f position, sf::Vector2f direction, bool active);
	unsigned int WorldID() const;
	sf::Vector2f GetPosition() const;
	sf::Vector2f GetDirection() const;
	bool IsActive() const;
};
