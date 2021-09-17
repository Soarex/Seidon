#include "WorkManager.h"

namespace Seidon
{
    std::vector<std::thread> WorkManager::threads;
    BlockingQueue<std::function<void(void)>> WorkManager::tasks;
    std::queue<std::function<void(void)>> WorkManager::mainThreadTasks;

    std::mutex WorkManager::mutex;

    void WorkManager::Init()
    {
        auto threadFunction = []()
        {
            while (true)
            {
                std::function<void(void)> task = tasks.Pop();
                task();
            }
        };

        for (int i = 0; i < std::thread::hardware_concurrency() - 1; i++)
            threads.push_back(std::thread(threadFunction));
    }

    void WorkManager::Destroy()
    {
        for (std::thread& thread : threads)
        {
            thread.detach();
        }
    }

    void WorkManager::Execute(const std::function<void(void)>& task)
    {
        tasks.Push(task);
    }

    void WorkManager::ExecuteOnMainThread(const std::function<void(void)>& task)
    {
        std::unique_lock<std::mutex> lock(mutex);
        mainThreadTasks.push(task);
    }

    void WorkManager::Update()
    {
        std::function<void(void)> task;
        for (int i = 0; i < mainThreadTasks.size(); i++)
        {
            {
                std::unique_lock<std::mutex> lock(mutex);
                task = mainThreadTasks.front();
                mainThreadTasks.pop();
            }
            task();
        }
    }
}