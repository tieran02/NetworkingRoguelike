#pragma once
#include "Message.h"
#include "SFML/Graphics.hpp"

class BroadcastMessage : public Message
{
public:
	//Size of message includng header
	const static size_t SIZE;

	BroadcastMessage() = default;;
    BroadcastMessage(char* buffer);
};
