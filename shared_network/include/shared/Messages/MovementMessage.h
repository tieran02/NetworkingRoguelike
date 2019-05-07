#pragma once
#include "Message.h"
#include "SFML/Graphics.hpp"

class MovementMessage : public Message
{
public:
	MovementMessage() = default;;
	MovementMessage(char* buffer);
	MovementMessage(unsigned int worldID, sf::Vector2f velocity, unsigned int senderID);
	unsigned int WorldID() const;
	sf::Vector2f GetVelocity() const;
};
