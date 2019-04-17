#pragma once
#include <vector>
#include "Message.h"
#include "../../../server/src/Network.h"
#include "BatchMessage.h"

template <typename T>
class MessageBatcher
{
public:
	MessageBatcher(int size);

	void AddMessage(const T& message);

	void SentToAllTCP(Network& network);
	void SentToAllUDP(Network& network);
private:
	std::vector<T> m_messages;

	std::vector<BatchMessage<T>> createBatches();
};

template <typename T>
MessageBatcher<T>::MessageBatcher(int size)
{
	m_messages.reserve(size);
}

template <typename T>
void MessageBatcher<T>::AddMessage(const T& message)
{
	m_messages.push_back(message);
}

template <typename T>
void MessageBatcher<T>::SentToAllTCP(Network& network)
{
	auto batches = createBatches();
	for (auto& batch : batches)
	{
		network.SendToAllTCP(batch);
	}
}

template <typename T>
void MessageBatcher<T>::SentToAllUDP(Network& network)
{
	auto batches = createBatches();
	for (auto& batch : batches)
	{
		network.SendToAllTCP(batch);
	}
}

template <typename T>
std::vector<BatchMessage<T>> MessageBatcher<T>::createBatches()
{
	//calculate number of batch messages needed
	const size_t totalSize = m_messages.size() * sizeof(T);
	const size_t maxSize{256};
	const int amountOfBatches = (totalSize - 1) / maxSize + 1;
	std::vector<BatchMessage<T>> batches(amountOfBatches);

	size_t currentBatchSize = 0;
	int currentBatchIndex = 0;

	for (auto& message : m_messages)
	{
		if (currentBatchSize + sizeof(T) > maxSize)
		{
			//add to new batch
			currentBatchSize = 0;
			currentBatchIndex++;
		}

		batches[currentBatchIndex].AddMessage(message);
		currentBatchSize += sizeof(T);
	}

	return batches;
}

