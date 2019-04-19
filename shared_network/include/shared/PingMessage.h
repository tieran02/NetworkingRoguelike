#pragma once
#include "Message.h"
#include <chrono>

class PingMessage : public Message
{
public:
	PingMessage() = default;;
	PingMessage(char* buffer);
	PingMessage(long long timestamp, unsigned int senderID);

	long long GetTimeStamp() const;
};
