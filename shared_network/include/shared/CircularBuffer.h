#pragma once
#include <cstdint>
#include <mutex>
#include <condition_variable>

template<class T>
class CircularBuffer
{
public:
	CircularBuffer(size_t size) : m_buffer(std::unique_ptr<T[]>(new T[size])) , m_capacity(size), m_front(0), m_back(0), m_count(0){}

	bool empty() const;

	void enqueue(const T& element);
	T dequeue();
private:
	std::unique_ptr<T[]> m_buffer;
	size_t m_capacity;
	uint32_t m_front;
	uint32_t m_back;
	uint32_t m_count;

	std::mutex m_mutex;
	std::condition_variable m_notFull;
	std::condition_variable m_notEmpty;
};

template <class T>
bool CircularBuffer<T>::empty() const
{
	return m_count == 0;
}

template <class T>
void CircularBuffer<T>::enqueue(const T& element)
{
	std::unique_lock<std::mutex> lock(m_mutex);

	m_notFull.wait(lock,[this] ()
	{
		return m_count != m_capacity;
	});

	m_buffer[m_back] = element;
	m_back = (m_back + 1) % m_capacity;
	++m_count;

	m_notEmpty.notify_one();
}

template <class T>
T CircularBuffer<T>::dequeue()
{
	std::unique_lock<std::mutex> lock(m_mutex);

	m_notEmpty.wait(lock,[this]()
	{
		return m_count != 0;
	});

	T& result = m_buffer[m_front];
	m_front = (m_front + 1) % m_capacity;
	--m_count;

	m_notFull.notify_one();
	return result;
}
