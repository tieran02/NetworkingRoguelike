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
	CONNECTION_ID,
	TEXT,
	MOVEMENT
};

struct Header
{
	int16_t size;
	MessageType type;
	uint16_t id{0};
};

class Message
{
public:
	Message() {}
	~Message(){}

	Message(char* buffer);

	Message(MessageType type, char* src, size_t size, uint16_t id);

	const Header& GetHeader() const;

	const char* GetData() const;

	std::string ToString() const;

	std::vector<char> GetBuffer() const;

	size_t Size() const;

protected:
	Header header{};
	std::vector<char> data;

};

//Message wrapper for the client/server. Includes the protocol, address and port of the sender
struct ServerMessage
{
	ServerMessage(){}
	ServerMessage(const Message& msg) : message(msg)
	{

	}

	Message message;
	Protocol protocol;
	sf::IpAddress senderAddress;
	unsigned short senderPort;
};
