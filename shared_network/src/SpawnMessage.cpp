#include "SpawnMessage.h"

SpawnMessage::SpawnMessage(char* buffer)
{

}

SpawnMessage::SpawnMessage(unsigned int worldID,unsigned int entityID, sf::Vector2f position, unsigned int ownerID)
{
    header.type = MessageType::SPAWN;
	header.size = sizeof(unsigned int) + sizeof(unsigned int) + sizeof(sf::Vector2f) + sizeof(unsigned int);

	data.resize(header.size);
	memcpy(data.data(), &worldID, sizeof(unsigned int));
	memcpy(data.data() + sizeof(unsigned int), &entityID, sizeof(unsigned int));
	memcpy(data.data() + sizeof(unsigned int) + sizeof(unsigned int), &position, sizeof(sf::Vector2f) );
	memcpy(data.data() + sizeof(unsigned int) + sizeof(unsigned int) + sizeof(sf::Vector2f), &ownerID, sizeof(unsigned int));
}

unsigned SpawnMessage::GetWorldID() const
{
	return *((unsigned int*)data.data());
}

unsigned int SpawnMessage::GetEntityID() const
{
	return *((unsigned int*)(data.data() + sizeof(unsigned int)));
}

sf::Vector2f SpawnMessage::GetPosition() const
{
	return *((sf::Vector2f*)(data.data() + sizeof(unsigned int) + sizeof(unsigned int)));
}

unsigned SpawnMessage::GetOwnershipID() const
{
	return *((unsigned int*)(data.data() + sizeof(unsigned int) + sizeof(unsigned int) + sizeof(sf::Vector2f)));
}
