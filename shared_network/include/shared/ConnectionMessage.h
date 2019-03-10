#pragma once
#include "Message.h"

class ConnectionMessage : public Message
{
public:
    ConnectionMessage(char* buffer);
    ConnectionMessage(unsigned int clientID, unsigned int seed);
    unsigned int GetClientID() const;
    unsigned int GetSeed() const;
};

