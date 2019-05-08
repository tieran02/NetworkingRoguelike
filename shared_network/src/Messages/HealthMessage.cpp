#include "Messages/HealthMessage.h"

//CANT SET SIZE OF DYNAMIC DATA SUCH AS STRINGS
const size_t HealthMessage::SIZE = sizeof(Header) + sizeof(unsigned int) + sizeof(float) + sizeof(float);

HealthMessage::HealthMessage(char* buffer) : Message(buffer)
{
}

HealthMessage::HealthMessage(unsigned int worldID, float health, float maxHealth, unsigned senderID)
{
	header.type = MessageType::HEALTH;
	header.size = sizeof(unsigned int) + sizeof(float) + sizeof(float);
	header.id = senderID;

	data.resize(header.size);
	memcpy(data.data(), &worldID, sizeof(unsigned int));
	memcpy(data.data() + sizeof(unsigned int), &health, sizeof(float));
	memcpy(data.data() + sizeof(unsigned int) + sizeof(float), &maxHealth, sizeof(float));
}

unsigned int HealthMessage::GetWorldID() const
{
	return *((unsigned int*)data.data());
}

float HealthMessage::GetHealth() const
{
	return *((float*)(data.data() + sizeof(unsigned int)));


}

float HealthMessage::GetMaxHealth() const
{
	return *((float*)(data.data() + sizeof(unsigned int) + sizeof(float)));
}
