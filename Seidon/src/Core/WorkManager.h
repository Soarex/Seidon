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
		std::vector<std::thread> threads;
		BlockingQueue<std::function<void(void)>> tasks;
		std::queue<std::function<void(void)>> mainThreadTasks;
		
		std::mutex mutex;

	public:
		void Init();
		void Destroy();
		void Execute(const std::function<void(void)>& task);
		void ExecuteOnMainThread(const std::function<void(void)>& task);
		void Update();
	};
}