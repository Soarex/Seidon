#pragma once
#include <string>
#include <chrono>
#include <iostream>

class Timer
{
private:
	std::string name;
	std::chrono::time_point<std::chrono::steady_clock> start;
public:
	Timer(const std::string& name) 
		: name(name)
	{
		
	}

	~Timer()
	{
		
	}

	void Start()
	{
		start = std::chrono::high_resolution_clock::now();
	}

	void Stop()
	{
		auto end = std::chrono::high_resolution_clock::now();

		auto startTime = std::chrono::time_point_cast<std::chrono::microseconds>(start).time_since_epoch().count();
		auto endTime = std::chrono::time_point_cast<std::chrono::microseconds>(end).time_since_epoch().count();

		float duration = (endTime - startTime) * 0.001f;

		std::cout << name << ": " << duration << " ms" << std::endl;
	}
};	