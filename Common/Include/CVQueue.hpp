#pragma once

#include <queue>
#include <mutex>
#include <condition_variable>

namespace IMChat
{
    template<typename T>
    class CVQueue
    {
    public:
        CVQueue() = default;
        ~CVQueue() = default;
        CVQueue(const CVQueue&) = delete;
        CVQueue& operator = (const CVQueue&) = delete;

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
            {
                std::unique_lock lock(m_Mutex);
                m_Queue.push(value);
            }

            m_CV.notify_one();
        }

        T wait_and_pop()
        {
            std::unique_lock lock(m_Mutex);
            m_CV.wait(lock, [this]{return !m_Queue.empty(); });

            auto temp = std::move(m_Queue.front());
            m_Queue.pop();
            return temp;
        }

        size_t size()
        {
            std::unique_lock lock(m_Mutex);
            return m_Queue.size();
        }

        bool empty()
        {
            std::unique_lock lock(m_Mutex);
            return m_Queue.empty();
        }
    private:
        std::mutex m_Mutex;
        std::queue<T> m_Queue;
        std::condition_variable m_CV;
    };
}
