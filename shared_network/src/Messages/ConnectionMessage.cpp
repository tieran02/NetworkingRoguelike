#include "Messages/ConnectionMessage.h"

const size_t ConnectionMessage::SIZE = sizeof(Header);


ConnectionMessage::ConnectionMessage(char* buffer) : Message(buffer)
{

}

ConnectionMessage::ConnectionMessage(unsigned int clientID, const std::string& clientName, unsigned int seed, unsigned short udpPort)
{
    header.type = MessageType::CONNECTION_ID;
	header.size = sizeof(unsigned int) + sizeof(unsigned int) + sizeof(unsigned short) + sizeof(uint8_t) + clientName.size();

	//copy data to message data
	data.resize(header.size);
	memcpy(data.data(), &clientID, sizeof(unsigned int));
	memcpy(data.data() + sizeof(unsigned int), &seed, sizeof(unsigned int));
	memcpy(data.data() + sizeof(unsigned int) + sizeof(unsigned int), &udpPort, sizeof(unsigned short));

	uint8_t sizeOfName = (uint8_t)clientName.size();
	memcpy(data.data() + sizeof(unsigned int) + sizeof(unsigned int) + sizeof(unsigned short), &sizeOfName, sizeof(uint8_t)); //copy the size of the string
	memcpy(data.data() + sizeof(unsigned int) + sizeof(unsigned int) + sizeof(unsigned short) + sizeof(uint8_t), clientName.c_str(), clientName.size());
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
	return *((unsigned short*)(data.data() + sizeof(unsigned int) + sizeof(unsigned int)));
}

std::string ConnectionMessage::GetName() const
{
	uint8_t size = *((uint8_t*)(data.data() + sizeof(unsigned int) + sizeof(unsigned int) + sizeof(unsigned short)));
	const char* startPtr = (const char*)(data.data() + sizeof(unsigned int) + sizeof(unsigned int) + sizeof(unsigned short) + sizeof(uint8_t));
	std::string name;
	name.assign(startPtr, (size_t)size);
	return name;
}