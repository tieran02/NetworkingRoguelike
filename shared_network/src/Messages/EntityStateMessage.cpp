#include "Messages/EntityStateMessage.h"

EntityStateMessage::EntityStateMessage(char* buffer) : Message(buffer)
{

}

EntityStateMessage::EntityStateMessage(unsigned int worldID, sf::Vector2f position, sf::Vector2f velocity, bool active, bool destroy, unsigned int senderID)
{
	header.type = MessageType::ENTITY_STATE;
	header.size = sizeof(unsigned int) + sizeof(sf::Vector2f) + sizeof(sf::Vector2f) + sizeof(bool) + sizeof(bool);
	header.id = senderID;

	data.resize(header.size);
	memcpy(data.data(), &worldID, sizeof(unsigned int));
	memcpy(data.data() + sizeof(unsigned int), &position, sizeof(sf::Vector2f));
	memcpy(data.data() + sizeof(unsigned int) + sizeof(sf::Vector2f), &velocity, sizeof(sf::Vector2f));
	memcpy(data.data() + sizeof(unsigned int) + sizeof(sf::Vector2f) + sizeof(sf::Vector2f), &active, sizeof(bool));
	memcpy(data.data() + sizeof(unsigned int) + sizeof(sf::Vector2f) + sizeof(sf::Vector2f) + sizeof(bool), &destroy, sizeof(bool));

}

unsigned EntityStateMessage::WorldID() const
{
	return *((unsigned int*)data.data());
}

sf::Vector2f EntityStateMessage::GetPosition() const
{
	return *((sf::Vector2f*)(data.data() + sizeof(unsigned int)));
}

sf::Vector2f EntityStateMessage::GetVelocity() const
{
	return *((sf::Vector2f*)(data.data() + sizeof(unsigned int) + sizeof(sf::Vector2f)));;
}

bool EntityStateMessage::IsActive() const
{
	return *((bool*)(data.data() + sizeof(unsigned int) + sizeof(sf::Vector2f) + sizeof(sf::Vector2f)));
}

bool EntityStateMessage::ShouldDestroy() const
{
	return *((bool*)(data.data() + sizeof(unsigned int) + sizeof(sf::Vector2f) + sizeof(sf::Vector2f) + sizeof(bool)));
}
