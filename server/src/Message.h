#pragma once
#include <SFML/Network.hpp>

enum class Protocol
{
	UPD,
	TCP
};

enum class MessageType
{
	TEXT,
	MOVEMENT
};

struct Header
{
	int16_t size;
	MessageType type;
};

struct Message
{
	Header header{};
	char* data{ nullptr };
	Protocol protocol;
	sf::IpAddress senderAddress;
	unsigned short senderPort;

	Message() {}

	Message(MessageType type, char* src, size_t size)
	{
		header.type = type;
		header.size = (int16_t)size;
		data = src;
	}

	Message(std::string& text)
	{
		header.type = MessageType::TEXT;
		header.size = (int16_t)text.size();

		//copy string data
		data = (char*)text.data();
	}

	std::string ToString()
	{
		return std::string(data);
	}
};

inline sf::Packet& operator <<(sf::Packet& packet, const Message& message)
{
	packet << static_cast<std::underlying_type<Protocol>::type>(message.header.type)
		<< message.header.size;

	packet.append(message.data, message.header.size);

	return packet;
}

inline sf::Packet& operator >>(sf::Packet& packet, Message& message)
{
	packet >> reinterpret_cast<std::underlying_type<Protocol>::type&>(message.header.type)
		>> message.header.size;

	//copy data to message
	message.data = ((char*)packet.getData() + sizeof(Protocol) + sizeof(int16_t));

	return packet;
}