#include "Messages/Message.h"

Message::Message(char* buffer)
{
	Header* headerPtr = (Header*)buffer;
	memcpy(&header, buffer, sizeof(Header));
	buffer += sizeof(Header);

	//copy data to message data
	data.resize(header.size);
	memcpy(data.data(), buffer, header.size);
}

Message::Message(MessageType type, char* src, size_t size, uint16_t id)
{
	header.type = type;
	header.size = (int16_t)size;
	header.id = id;

	//copy data to message data
	data.resize(header.size);
	memcpy(data.data(), src, header.size);
}

const Header& Message::GetHeader() const
{
	return header;
}

const char* Message::GetData() const
{
	return data.data();
}

std::string Message::ToString() const
{
	std::string str;
	str.assign(data.data(), header.size);
	return str;
}

size_t Message::Size() const
{
    return sizeof(Header) + header.size;
}

std::vector<char> Message::GetBuffer() const
{
	std::vector<char> buffer(sizeof(Header) + header.size);
	memcpy(buffer.data(), &header, sizeof(header));
	if(header.size > 0)
		memcpy(buffer.data() + sizeof(Header), data.data(), header.size);
	return buffer;
}
