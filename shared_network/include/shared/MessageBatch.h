#pragma once
#include "Message.h"

template <typename T>
class BatchMessage : public Message
{
public:
	BatchMessage() = default;
	BatchMessage(char* buffer);
	BatchMessage(MessageType type, int size);

	T& operator[] (size_t n);
private:
	MessageType m_type;
	int m_size;
};

template <typename T>
BatchMessage<T>::BatchMessage(char* buffer) : Message(buffer)
{
}

template <typename T>
BatchMessage<T>::BatchMessage(MessageType type, int size)
{
	m_size = size;
	m_type = type;

	header.size = sizeof(T) * m_size;
	header.type = MessageType::BATCH;

	data.resize(header.size);
}

template <typename T>
T& BatchMessage<T>::operator[](size_t n)
{
	return *((T*)(data.data() + (n * sizeof(T))));
}
