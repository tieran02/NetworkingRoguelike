#include "Messages/PingMessage.h"

//CANT SET SIZE OF DYNAMIC DATA SUCH AS STRINGS
const size_t PingMessage::SIZE = sizeof(Header) + sizeof(long long);

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
