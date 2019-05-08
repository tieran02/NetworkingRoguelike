#include "Messages/NetworkMessages.h"
#include "Utility/Log.h"

BatchMessage::BatchMessage(char * buffer) : Message(buffer)
{

}

BatchMessage::BatchMessage(uint8_t count, size_t sizeOfMessage)
{
	header.type = MessageType::BATCH;
	header.size = sizeof(uint8_t) + ((uint16_t)sizeOfMessage * count);

	data.resize(header.size);
	memcpy(data.data(), &count, sizeof(uint8_t));
}

void BatchMessage::InsertMessageAt(int index, const Message& message)
{
	if (index >= (int)GetCount())
	{
		LOG_FATAL("FAILED TO INSERT MESSAGE INTO BATCH (INDEX IS GREATER THAN COUNT)");
		return;
	}

	auto buffer = message.GetBuffer(); //copy the buffer into the batch message
	memcpy(data.data() + sizeof(uint8_t) + (index*buffer.size()), buffer.data(), buffer.size()); // insert header
}

const uint8_t BatchMessage::GetCount() const
{
	return *((uint8_t*)(data.data()));
}

const Message BatchMessage::GetMessageAt(int index) const
{
	if (index >= (int)GetCount()) 
	{
		LOG_FATAL("FAILED TO ACCESS MESSAGE IN BATCH (INDEX IS GREATER THAN COUNT)");
		return Message();
	}

	Header* header = (Header*)(data.data() + sizeof(uint8_t));
	//get size of the message
	uint16_t sizeOfMessage = sizeof(Header) + header->size;

	std::vector<char> buffer(sizeOfMessage);
	memcpy(buffer.data(), data.data() + sizeof(uint8_t) + (index*sizeOfMessage), sizeOfMessage);

	Message msg(buffer.data());
	return msg;
}
