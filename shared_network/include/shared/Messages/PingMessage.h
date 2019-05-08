#pragma once
#include "Message.h"
#include <chrono>

class PingMessage : public Message
{
public:
	//Size of message includng header
	const static size_t SIZE;

	PingMessage() = default;;
	PingMessage(char* buffer);
	PingMessage(long long timestamp, unsigned int senderID);

	long long GetTimeStamp() const;
};
