#include "Messages/HealthMessage.h"

HealthMessage::HealthMessage(char* buffer) : Message(buffer)
{
}

HealthMessage::HealthMessage(float amount, unsigned senderID)
{
	header.type = MessageType::PING;
	header.size = sizeof(float);
	header.id = senderID;

	data.resize(header.size);
	memcpy(data.data(), &amount, sizeof(float));
}
