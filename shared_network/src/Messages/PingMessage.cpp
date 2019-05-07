#include "Messages/PingMessage.h"

PingMessage::PingMessage(char* buffer) : Message(buffer)
{

}

PingMessage::PingMessage(long long  timestamp, unsigned int senderID)
{
	header.type = MessageType::PING;
	header.size = sizeof(long long);
	header.id = senderID;

	data.resize(header.size);
	memcpy(data.data(), &timestamp, sizeof(long long));

}

long long PingMessage::GetTimeStamp() const
{
	return *((long long*)data.data());
}
