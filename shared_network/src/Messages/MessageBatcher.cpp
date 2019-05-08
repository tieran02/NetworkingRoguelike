#include "Messages/MessageBatcher.h"

constexpr int MAX_MESSAGE_SIZE{ 256 };

MessageBatcher::MessageBatcher(size_t sizeOfMessage) :
	m_sizeOfMessage(sizeOfMessage)
{
}

MessageBatcher::~MessageBatcher()
{
}

void MessageBatcher::AddMessage(Message * message)
{
	m_messages.push_back(message);
}

void MessageBatcher::BatchMessages()
{
	const size_t TOTAL_MESSAGE_COUNT = m_messages.size();
	const size_t TOTAL_MESSAGE_SIZE = TOTAL_MESSAGE_COUNT * m_sizeOfMessage;
	const size_t MAX_MESSAGE_PAYLOAD = MAX_MESSAGE_SIZE - sizeof(uint8_t);

	//calculate how many batches are required
	const int BATCH_COUNT = ((int)TOTAL_MESSAGE_SIZE - 1) / ((int)MAX_MESSAGE_PAYLOAD) + 1;
	const int MESSAGES_PER_BATCH = (int)TOTAL_MESSAGE_COUNT / (int)BATCH_COUNT;
	const int MESSAGES_IN_LAST_BATCH = (int)TOTAL_MESSAGE_COUNT % (int)BATCH_COUNT;

	if (BATCH_COUNT == 1) //Only One batch is needed
	{
		m_batches.push_back(BatchMessage{ (uint8_t)MESSAGES_IN_LAST_BATCH,m_sizeOfMessage });
		for (size_t i = 0; i < MESSAGES_IN_LAST_BATCH; i++)
		{
			m_batches[0].InsertMessageAt((int)i, m_messages[i], m_sizeOfMessage);
		}
	}
	else //multiple batches needed
	{
		for (size_t batchIndex = 0; batchIndex < BATCH_COUNT; batchIndex++)
		{
			//check if its the last batch
			if (batchIndex == BATCH_COUNT - 1)
			{
				m_batches.push_back(BatchMessage{ (uint8_t)MESSAGES_IN_LAST_BATCH, m_sizeOfMessage });
				for (size_t i = 0; i < MESSAGES_IN_LAST_BATCH; i++)
				{
					int messageIndex = (int)i + ((int)batchIndex * MESSAGES_PER_BATCH);
					m_batches[batchIndex].InsertMessageAt((int)i , m_messages[messageIndex], m_sizeOfMessage);
				}
			}
			else 
			{
				m_batches.push_back(BatchMessage{ (uint8_t)MESSAGES_PER_BATCH, m_sizeOfMessage });
				for (size_t i = 0; i < MESSAGES_PER_BATCH; i++)
				{
					int messageIndex = (int)i + ((int)batchIndex * MESSAGES_PER_BATCH);
					m_batches[batchIndex].InsertMessageAt((int)i, m_messages[messageIndex], m_sizeOfMessage);
				}
			}
		}
	}
}

const std::vector<BatchMessage>& MessageBatcher::GetBatches() const
{
	return m_batches;
}
