#pragma once
#include <SFML/Network.hpp>
#include <cstring>
#include <iostream>

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
public:
	Message() {}
	~Message()
	{
	}

	//Create a message from a message buffer
	Message(char* buffer)
	{
		Header* headerPtr = (Header*)buffer;
		memcpy(&header, buffer, sizeof(Header));
		buffer += sizeof(Header);

		//copy data to message data
		data.resize(header.size);
		memcpy(data.data(), buffer, header.size);
	}

	//Create a message with a set of data
	Message(MessageType type, char* src, size_t size)
	{
		header.type = type;
		header.size = (int16_t)size;

		//copy data to message data
		data.resize(header.size);
		memcpy(data.data(), src, header.size);
	}

	const Header& GetHeader() const
	{
		return header;
	}

	const char* GetData() const
	{
		return data.data();
	}

	std::string ToString() const
	{
		std::string str;
		str.assign(data.data(), header.size);
		return str;
	}

	//Create a buffer containing the header and the data
	std::vector<char> GetBuffer() const
	{
		std::vector<char> buffer(sizeof(Header) + header.size);
		memcpy(buffer.data(), &header, sizeof(header));
		memcpy(buffer.data() + sizeof(Header), data.data(), header.size);
		return buffer;
	}

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
};
