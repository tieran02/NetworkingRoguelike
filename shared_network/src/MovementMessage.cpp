#include "MovementMessage.h"

MovementMessage::MovementMessage(char* buffer) : Message(buffer)
{

}

MovementMessage::MovementMessage(unsigned int worldID, sf::Vector2f position, sf::Vector2f direction)
{
    header.type = MessageType::MOVEMENT;
	header.size = sizeof(unsigned int) + sizeof(sf::Vector2f)*2;

	data.resize(header.size);
	memcpy(data.data(), &worldID, sizeof(unsigned int));
	memcpy(data.data() + sizeof(unsigned int), &position, sizeof(sf::Vector2f));
	memcpy(data.data() + sizeof(unsigned int) + sizeof(sf::Vector2f), &direction, sizeof(sf::Vector2f) );
}

unsigned MovementMessage::WorldID() const
{
	return *((unsigned int*)data.data());
}

sf::Vector2f MovementMessage::GetPosition() const
{
	return *((sf::Vector2f*)(data.data() + sizeof(unsigned int)));;
}

sf::Vector2f MovementMessage::GetDirection() const
{
	return *((sf::Vector2f*)(data.data() + sizeof(unsigned int) + sizeof(sf::Vector2f)));;
}
