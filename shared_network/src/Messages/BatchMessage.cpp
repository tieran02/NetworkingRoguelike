#include "Messages/BatchMessage.h"
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

void BatchMessage::InsertMessageAt(int index, Message* message, size_t sizeOfMessage)
{
	if (index >= (int)GetCount())
	{
		LOG_FATAL("FAILED TO INSERT MESSAGE INTO BATCH (INDEX IS GREATER THAN COUNT)");
		return;
	}

	memcpy(data.data() + sizeof(uint8_t), message, sizeOfMessage);
}

const uint8_t BatchMessage::GetCount() const
{
	return *((uint8_t*)(data.data()));
}

const Message* BatchMessage::GetMessageAt(int index) const
{
	if (index >= (int)GetCount()) 
	{
		LOG_FATAL("FAILED TO ACCESS MESSAGE IN BATCH (INDEX IS GREATER THAN COUNT)");
		return nullptr;
	}

	Message* firstMessage = (Message*)(data.data() + sizeof(uint8_t));
	//get size of the message
	uint16_t sizeOfMessage = firstMessage->GetHeader().size;

	return firstMessage + sizeOfMessage * index;
}
