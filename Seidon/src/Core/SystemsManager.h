#pragma once

#include <map>
#include <string>
#include <typeinfo>

#include "System.h"

namespace Seidon
{
	class SystemsManager
	{
	private:
		static std::map<std::string, System*> systems;

	public:
		static void Init();

		template <typename T, typename... Args>
		static T& AddSystem(Args&&... args)
		{
			systems[typeid(T).name()] = new T(std::forward<Args>(args)...);
			systems[typeid(T).name()]->Setup();
			return *((T*)systems[typeid(T).name()]);
		}

		template <typename T>
		static T& GetSystem()
		{
			return *((T*)systems[typeid(T).name()]);
		}

		template <typename T>
		static void RemoveSystem()
		{
			systems.erase(typeid(T).name());
		}

		static void Update(float deltaTime);
	};
}