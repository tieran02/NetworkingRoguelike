#pragma once
#include "Message.h"

class HealthMessage : public Message
{
public:
	//Size of message includng header
	const static size_t SIZE;

	HealthMessage() = default;;
	HealthMessage(char* buffer);
	HealthMessage(unsigned int worldID, float health, float maxHealth, unsigned int senderID);

	unsigned int GetWorldID() const;
	float GetHealth() const;
	float GetMaxHealth() const;
};

