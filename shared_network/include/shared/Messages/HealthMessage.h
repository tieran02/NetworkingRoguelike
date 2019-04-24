#pragma once
#include "Message.h"

class HealthMessage : public Message
{
public:
	HealthMessage() = default;;
	HealthMessage(char* buffer);
	HealthMessage(unsigned int worldID, float health, float maxHealth, unsigned int senderID);

	unsigned int GetWorldID() const;
	float GetHealth() const;
	float GetMaxHealth() const;
};

