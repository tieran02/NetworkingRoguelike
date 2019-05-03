#pragma once
#include <vector>
#include <thread>
#include <condition_variable>
#include <mutex>
#include <queue>


class ThreadPool
{
public:
	using Task = std::function<void()>;


	ThreadPool(size_t numThreads);
	~ThreadPool();

	void enqueue(Task task);
private:
	std::vector<std::thread> m_threads;

	std::condition_variable m_eventCV;
	std::mutex m_eventMutex;
	bool m_stopping{false};

	std::queue<Task> m_tasks;

	void start(size_t numThreads);
	void stop() noexcept;
};

