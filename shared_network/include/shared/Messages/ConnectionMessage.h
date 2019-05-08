#pragma once
#include "Message.h"

class ConnectionMessage : public Message
{
public:
	//Size of message includng header
	const static size_t SIZE;

	ConnectionMessage() = default;;
    ConnectionMessage(char* buffer);
    ConnectionMessage(unsigned int clientID, const std::string& clientName, unsigned int seed, unsigned short udpPort);
    unsigned int GetClientID() const;
	std::string GetName() const;
    unsigned int GetSeed() const;
	unsigned short GetUdpPort() const;
};

