#include "Messages/BroadcastMessage.h"

const size_t BroadcastMessage::SIZE = sizeof(Header);

BroadcastMessage::BroadcastMessage(char* buffer) : Message(buffer)
{

}
