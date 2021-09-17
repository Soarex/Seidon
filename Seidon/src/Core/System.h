#pragma once
#include <iostream>
namespace Seidon
{
	class System
	{
	public:
		virtual void Setup() {};
		virtual void Update(float deltaTime) {};
	};
}