#pragma once
#include "Message.h"
#include "SFML/Graphics.hpp"

class BroadcastMessage : public Message
{
public:
    BroadcastMessage(char* buffer);
    BroadcastMessage();
};
