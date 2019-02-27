#pragma once

#include "Message.h"
#include <queue>
#include <mutex>
#include <condition_variable>

class MessageQueue
{
public:
    MessageQueue();
    ~MessageQueue();

    void enqueue(const ServerMessage& message);
    ServerMessage dequeue();
    bool empty() const;

private:
    std::queue<ServerMessage> m_queue;
    mutable std::mutex m_mutex;
    std::condition_variable m_condition;
};
