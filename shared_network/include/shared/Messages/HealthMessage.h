#pragma once
#include "Messages/Message.h"

class HealthMessage : Message
{
public:
	HealthMessage() = default;;
	HealthMessage(char* buffer);
	HealthMessage(float amount, unsigned int senderID);
	
	float GetAmount() const;
};

