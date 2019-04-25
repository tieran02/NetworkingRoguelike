#include "ThreadPool.h"
#include "Utility/Log.h"


ThreadPool::ThreadPool(size_t numThreads)
{
	start(numThreads);
}

ThreadPool::~ThreadPool()
{
	stop();
}

void ThreadPool::enqueue(Task task)
{
	std::unique_lock<std::mutex> lock{ m_eventMutex };
	m_tasks.emplace(std::move(task));
	lock.unlock();

	m_eventCV.notify_one();
}

void ThreadPool::start(size_t numThreads)
{
	LOG_INFO("Creating Thread Pool");
	for (size_t i = 0; i < numThreads; ++i)
	{
		m_threads.emplace_back([=]
		{
			while (true)
			{
				Task task;


				{
					std::unique_lock<std::mutex> lock{ m_eventMutex };

					m_eventCV.wait(lock, [=] {return  m_stopping || !m_tasks.empty(); });
					if (m_stopping)
						break;

					task = std::move(m_tasks.front());
					m_tasks.pop();
				}

				task();
			}
		});
	}
}

void ThreadPool::stop() noexcept
{
	{
		std::unique_lock<std::mutex> lock{ m_eventMutex };
		m_stopping = true;
	}

	m_eventCV.notify_all();

	for (auto& thread : m_threads)
	{
		thread.join();
	}

}
