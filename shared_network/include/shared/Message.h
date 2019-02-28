#pragma once
#include <SFML/Network.hpp>
#include <cstring>

enum class Protocol
{
	UPD,
	TCP
};

enum class MessageType
{
	BROADCAST,
	BROADCAST_RESPONSE,
	CONNECTION_ID,
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
	Message() {}
	~Message(){}

	Message(char* buffer);

	Message(MessageType type, char* src, size_t size);

	const Header& GetHeader() const;

	const char* GetData() const;

	std::string ToString() const;

	std::vector<char> GetBuffer() const;

private:
	Header header{};
	std::vector<char> data;

};

//Message wrapper for the client/server. Includes the protocol, address and port of the sender
struct ServerMessage
{
	ServerMessage(const Message& msg) : message(msg)
	{

	}

	const Message message;
	Protocol protocol;
	sf::IpAddress senderAddress;
	unsigned short senderPort;
	unsigned int clientID{ 0 };
};
