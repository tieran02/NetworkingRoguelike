#pragma once
#include <queue>
#include <mutex>

template <typename T>
class Queue
{
public:

	bool empty()
	{
		std::unique_lock<std::mutex> lock(m_mutex);
		return m_queue.empty();
	}

	T dequeue()
	{
		std::unique_lock<std::mutex> lock(m_mutex);
		m_cv.wait(lock, [this] {return !m_queue.empty(); });
		auto val = m_queue.front();
		m_queue.pop();
		return val;
	}

	//Non blocking deque
	T dequeue(T& item)
	{
		std::unique_lock<std::mutex> lock(m_mutex);
		if (m_queue.empty())
		{
			return;
		}
		item = m_queue.front();
		m_queue.pop();
	}

	void enqueue(const T& item)
	{
		std::unique_lock<std::mutex> lock(m_mutex);
		m_queue.push(item);
		m_cv.notify_one();
	}

	Queue() = default;
	Queue(const Queue&) = delete;
	Queue& operator=(const Queue&) = delete;
private:
	std::queue<T> m_queue;
	std::mutex m_mutex;
	std::condition_variable m_cv;
};


