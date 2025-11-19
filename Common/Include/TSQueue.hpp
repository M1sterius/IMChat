#pragma once

#include <queue>
#include <mutex>

namespace IMChat
{
    /**
     * Thread-safe queue class
     */
    template<typename T>
    class TSQueue
    {
    public:
        TSQueue() = default;
        ~TSQueue() = default;
        TSQueue(const TSQueue&) = delete;
        TSQueue& operator = (const TSQueue&) = delete;

        const T& front()
        {
            std::unique_lock lock(m_Mutex);
            return m_Queue.front();
        }

        const T& back()
        {
            std::unique_lock lock(m_Mutex);
            return m_Queue.back();
        }

        void push(const T& value)
        {
            std::unique_lock lock(m_Mutex);
            m_Queue.push(value);
        }

        void pop()
        {
            std::unique_lock lock(m_Mutex);
            m_Queue.pop();
        }

        const T& pop_front()
        {
            std::unique_lock lock(m_Mutex);
            const auto& temp = m_Queue.front();
            m_Queue.pop();
            return temp;
        }

        size_t size()
        {
            std::unique_lock lock(m_Mutex);
            return m_Queue.size();
        }
    private:
        std::mutex m_Mutex;
        std::queue<T> m_Queue;
    };
}
