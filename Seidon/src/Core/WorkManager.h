#pragma once
#include <thread>
#include <vector>
#include <functional>

#include "Utils/BlockingQueue.h"

namespace Seidon
{
	class WorkManager
	{
	private: 
		static  std::vector<std::thread> threads;
		static  BlockingQueue<std::function<void(void)>> tasks;
		static  std::queue<std::function<void(void)>> mainThreadTasks;
		
		static  std::mutex mutex;

	public:
		static void Init();
		static void Destroy();
		static void Execute(const std::function<void(void)>& task);
		static void ExecuteOnMainThread(const std::function<void(void)>& task);
		static void Update();
	};
}