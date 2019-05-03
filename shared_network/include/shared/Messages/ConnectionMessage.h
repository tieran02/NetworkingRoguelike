#pragma once
#include "Message.h"

class ConnectionMessage : public Message
{
public:
	ConnectionMessage() = default;;
    ConnectionMessage(char* buffer);
    ConnectionMessage(unsigned int clientID, unsigned int seed, unsigned short udpPort);
    unsigned int GetClientID() const;
    unsigned int GetSeed() const;
	unsigned short GetUdpPort() const;
};

