#pragma once

#include <iostream>
#include <algorithm>
#include <queue>
#include <mutex>
#include <thread>
#include <condition_variable>

namespace Seidon
{
    template<typename T>
    class BlockingQueue {
    private:
        std::queue<T> queue;
        std::mutex mutex;
        std::condition_variable emptyWaitCondition;

    public:
        inline int GetSize()
        {
            std::unique_lock<std::mutex> lock(mutex);
            return queue.size();
        }

        inline void Push(T item)
        {
            {
                std::unique_lock<std::mutex> lock(mutex);
                queue.push(item);
            }
            emptyWaitCondition.notify_one();
        }

        inline T Pop()
        {
            std::unique_lock<std::mutex> lock(mutex);

            while (queue.size() == 0)
                emptyWaitCondition.wait(lock);

            T res = queue.front();
            queue.pop();
            return res;
        }
    };
}