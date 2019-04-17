#pragma once
#include "Message.h"
#include "SFML/Graphics.hpp"

class BroadcastMessage : public Message
{
public:
	BroadcastMessage() = default;;
    BroadcastMessage(char* buffer);
};
