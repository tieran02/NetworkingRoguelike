#include "Messages/SpawnMessage.h"
#include "./Utility/Log.h"

SpawnMessage::SpawnMessage(char* buffer) : Message(buffer)
{
    LOG_INFO("Creating Spawn message from buffer" + std::to_string(GetWorldID()));
}

SpawnMessage::SpawnMessage(unsigned int worldID, unsigned int entityID, sf::Vector2f position, sf::Vector2f velocity, unsigned int ownerID, CollisionLayer layerOverride)
{
    header.type = MessageType::SPAWN;
	header.size = sizeof(unsigned int) + sizeof(unsigned int) + sizeof(sf::Vector2f) + sizeof(sf::Vector2f) + sizeof(unsigned int) + sizeof(CollisionLayer);

	data.resize(header.size);
    memcpy(data.data(), &worldID, sizeof(unsigned int));
	memcpy(data.data() + sizeof(unsigned int), &entityID, sizeof(unsigned int));
	memcpy(data.data() + sizeof(unsigned int) + sizeof(unsigned int), &position, sizeof(sf::Vector2f) );
	memcpy(data.data() + sizeof(unsigned int) + sizeof(unsigned int) + sizeof(sf::Vector2f), &velocity, sizeof(sf::Vector2f));
	memcpy(data.data() + sizeof(unsigned int) + sizeof(unsigned int) + sizeof(sf::Vector2f) + sizeof(sf::Vector2f), &ownerID, sizeof(unsigned int));
	memcpy(data.data() + sizeof(unsigned int) + sizeof(unsigned int) + sizeof(sf::Vector2f) + sizeof(sf::Vector2f) + sizeof(unsigned int), &layerOverride, sizeof(CollisionLayer));

	LOG_INFO("Creating Spawn message" + std::to_string(GetWorldID()));

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

sf::Vector2f SpawnMessage::GetVelocity() const
{
	return *((sf::Vector2f*)(data.data() + sizeof(unsigned int) + sizeof(unsigned int) + sizeof(sf::Vector2f)));
}

unsigned SpawnMessage::GetOwnershipID() const
{
	return *((unsigned int*)(data.data() + sizeof(unsigned int) + sizeof(unsigned int) + sizeof(sf::Vector2f) + sizeof(sf::Vector2f)));
}

CollisionLayer SpawnMessage::GetLayerOverride() const
{
	return *((CollisionLayer*)(data.data() + sizeof(unsigned int) + sizeof(unsigned int) + sizeof(sf::Vector2f) + sizeof(sf::Vector2f) + sizeof(unsigned int)));
}
