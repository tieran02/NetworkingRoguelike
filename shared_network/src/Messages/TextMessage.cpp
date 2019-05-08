#include "Messages/TextMessage.h"

//CANT SET SIZE OF DYNAMIC DATA SUCH AS STRINGS
const size_t TextMessage::SIZE = sizeof(Header);

TextMessage::TextMessage(char* buffer) : Message(buffer)
{

}

TextMessage::TextMessage(const std::string& text, TextType textType, unsigned int senderID)
{
	header.type = MessageType::TEXT;
	header.size = sizeof(TextType) + sizeof(uint8_t) + (uint8_t)text.size();
	header.id = senderID;

	//copy data to message data
	data.resize(header.size);
	memcpy(data.data(), &textType, sizeof(TextType));

	uint8_t sizeOfName = (uint8_t)text.size();
	memcpy(data.data() + sizeof(TextType), &sizeOfName, sizeof(uint8_t)); //copy the size of the string
	memcpy(data.data() + sizeof(TextType) + sizeof(uint8_t), text.c_str(), text.size());
}

TextType TextMessage::GetTextType() const
{
	return *((TextType*)data.data());
}

std::string TextMessage::GetText() const
{
	uint8_t size = *((uint8_t*)(data.data() + sizeof(TextType)));
	const char* startPtr = (const char*)(data.data() + sizeof(TextType) + sizeof(uint8_t));
	std::string name;
	name.assign(startPtr, (size_t)size);
	return name;
}