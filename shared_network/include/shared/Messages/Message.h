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
	SPAWN,
	TEXT,
	MOVEMENT,
	ENTITY_STATE,
	BATCH,
	DISCONNECT,
	CLIENT_SETUP,
	PING,
	HEALTH,
	COLLIDE,
	GAME_START
};

struct Header
{
	int16_t size;
	uint16_t id{ 0 };
	MessageType type;
};

class Message
{
public:
	Message() {}
	~Message() {}

	Message(char* buffer);
	//Copy message from another message pointer
	Message(Message* message);

	Message(MessageType type, char* src, size_t size, uint16_t id);

	const Header& GetHeader() const;

	const std::vector<char>& GetData() const;

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
	ServerMessage() {}
	ServerMessage(const Message& msg) : message(msg)
	{

	}

	Message message;
	Protocol protocol;
	sf::IpAddress senderAddress;
	unsigned short senderPort;
};
