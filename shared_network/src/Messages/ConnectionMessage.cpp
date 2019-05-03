#include "Messages/ConnectionMessage.h"

ConnectionMessage::ConnectionMessage(char* buffer) : Message(buffer)
{

}

ConnectionMessage::ConnectionMessage(unsigned int clientID, unsigned int seed, unsigned short udpPort)
{
    header.type = MessageType::CONNECTION_ID;
	header.size = sizeof(unsigned int) + sizeof(unsigned int) + sizeof(unsigned short);

	//copy data to message data
	data.resize(header.size);
	memcpy(data.data(), &clientID, sizeof(unsigned int));
	memcpy(data.data() + sizeof(unsigned int), &seed, sizeof(unsigned int));
	memcpy(data.data() + sizeof(unsigned int) + sizeof(unsigned int), &udpPort, sizeof(unsigned short));
}

unsigned int ConnectionMessage::GetClientID() const
{
    return *((unsigned int*)data.data());
}

unsigned int ConnectionMessage::GetSeed() const
{
    return *((unsigned int*)(data.data() + sizeof(unsigned int)));;

}

unsigned short ConnectionMessage::GetUdpPort() const
{
	return *((unsigned short*)(data.data() + sizeof(unsigned int) + sizeof(unsigned int)));;
}

