#pragma once
#include <vector>
#include "BatchMessage.h"

class MessageBatcher
{
public:
	MessageBatcher(size_t sizeOfMessage);
	~MessageBatcher();

	void AddMessage(Message* message);

	void BatchMessages();

	const std::vector<BatchMessage>& GetBatches() const;
private:
	//Size of the message including the header
	size_t m_sizeOfMessage;
	std::vector<Message*> m_messages;
	std::vector<BatchMessage> m_batches;
};

