#pragma once
#include "Message.h"

template <typename T>
class MessageBatch : public Message
{
public:
	MessageBatch(char* buffer);
	MessageBatch(MessageType type, int size);

	T& operator[] (int x);
private:
	MessageType m_type;
	int m_size;
	std::vector<char> m_data;
};

template <typename T>
MessageBatch<T>::MessageBatch(char* buffer) : Message(buffer)
{
}

template <typename T>
MessageBatch<T>::MessageBatch(MessageType type, int size)
{
	m_size = size;
	m_type = type;

	header.size = sizeof(T) * m_size;
	header.type = MessageType::BATCH;

	data.resize(header.size);
}

template <typename T>
T& MessageBatch<T>::operator[](int x)
{
	return *reinterpret_cast<T*>(data.data()[x * sizeof(T)]);
}

