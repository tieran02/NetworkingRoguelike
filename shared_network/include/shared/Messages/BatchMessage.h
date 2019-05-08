#pragma once
#include "Message.h"

class BatchMessage : public Message
{
public:
	BatchMessage() = default;
	BatchMessage(char* buffer);
	BatchMessage(uint8_t count, size_t sizeOfMessage);
	void InsertMessageAt(int index, const Message& message);

	const uint8_t GetCount() const;
	const Message GetMessageAt(int index) const;
};
