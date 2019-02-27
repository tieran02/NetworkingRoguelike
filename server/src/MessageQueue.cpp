#include "MessageQueue.h"

MessageQueue::MessageQueue() : m_queue{}, m_mutex{},m_condition{}
{
    //ctor
}

MessageQueue::~MessageQueue()
{
    //dtor
}

void MessageQueue::enqueue(const ServerMessage& message)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_queue.push(message);
    m_condition.notify_one();
}

ServerMessage MessageQueue::dequeue()
{
    std::unique_lock<std::mutex> lock(m_mutex);
    while(m_queue.empty())
    {
        //queue is empty so release the lock
        m_condition.wait(lock);
    }
    ServerMessage message = m_queue.front();
    m_queue.pop();
    return message;
}

bool MessageQueue::empty() const
{
    return m_queue.empty();
}
