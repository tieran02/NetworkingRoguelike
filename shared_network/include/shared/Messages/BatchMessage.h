#pragma once
#include "Message.h"

template <typename T>
class BatchMessage : public Message
{
public:
	BatchMessage();
	BatchMessage(char* buffer);
	BatchMessage(MessageType type);
	BatchMessage(MessageType type, int size);

	void AddMessage(const T& message);

	T& operator[] (size_t n);
private:
	MessageType m_type;
	int m_size;
};

template <typename T>
BatchMessage<T>::BatchMessage()
{
	m_size = 0;
	m_type = MessageType::BATCH;
	header.size = 0;
	header.type = MessageType::BATCH;
}

template <typename T>
BatchMessage<T>::BatchMessage(char* buffer) : Message(buffer)
{
}

template <typename T>
BatchMessage<T>::BatchMessage(MessageType type)
{
	m_size = 0;
	m_type = type;

	header.size = 0;
	header.type = type;
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
void BatchMessage<T>::AddMessage(const T& message)
{
	m_size++;
	header.size = sizeof(T) * m_size;
	data.resize(header.size);

	memcpy(data.data() + (sizeof(T) * (m_size - 1)), &message, sizeof(T));
}

template <typename T>
T& BatchMessage<T>::operator[](size_t n)
{
	return *((T*)(data.data() + (n * sizeof(T))));
}
