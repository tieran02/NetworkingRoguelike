#pragma once
#include <cstdint>
#include <mutex>

template<class T,uint32_t Size>
class CircularBuffer
{
public:
	CircularBuffer() : m_buffer(std::unique_ptr<T[]>(new T[Size])), m_front(0), m_back(0), m_full(false)
	{
	}

	bool empty() const;
	//Get the overall size of the buffer
	size_t capacity() const;
	//Get the current size of the buffer
	size_t size() const;

	void enqueue(const T& element);
	T& dequeue();
private:
	std::unique_ptr<T[]> m_buffer;
	uint32_t m_front;
	uint32_t m_back;
	bool m_full;

	std::mutex m_mutex;
};

template <class T, uint32_t Size>
bool CircularBuffer<T, Size>::empty() const
{
	return (!m_full && (m_front == m_back));
}

template <class T, uint32_t Size>
size_t CircularBuffer<T, Size>::capacity() const
{
	return Size;
}

template <class T, uint32_t Size>
size_t CircularBuffer<T, Size>::size() const
{
	size_t size = Size;

	if(!m_full)
	{
		if(m_front >= m_back)
		{
			size = m_front - m_back;
		}
		else
		{
			size = Size + m_front - m_back;
		}
	}

	return size;
}

template <class T, uint32_t Size>
void CircularBuffer<T, Size>::enqueue(const T& element)
{
	std::lock_guard<std::mutex> lock(m_mutex);

	m_buffer[m_front] = element;

	if(m_full)
	{
		m_back = (m_back + 1) % Size;
	}

	m_front = (m_front + 1) % Size;

	m_full = m_front == m_back;
}

template <class T, uint32_t Size>
T& CircularBuffer<T, Size>::dequeue()
{
	std::lock_guard<std::mutex> lock(m_mutex);

	if (empty())
	{
		return T();
	}

	T& value = m_buffer[m_back];
	m_full = false;
	m_back = (m_back + 1) % Size;

	return value;
}
