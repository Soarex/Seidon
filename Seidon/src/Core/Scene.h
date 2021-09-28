#pragma once

#include <map>
#include <string>
#include <typeinfo>

#include <entt/entt.hpp>
#include "System.h"


namespace Seidon
{
	class Entity;

	class Scene
	{
	private:
		std::string name;
		entt::registry registry;
		std::map<std::string, System*> systems;

	public:
		Scene(const std::string& name = "Scene");
		~Scene();

		void Init();
		void Update(float deltaTime);
		void Destroy();

		Entity CreateEntity(const std::string& name = std::string());
		void DestroyEntity(const Entity& entity);

		inline void SetName(const std::string& name) { this->name = name; }

		inline entt::registry& GetRegistry() { return registry; }
		inline const std::string& GetName() { return name; }

		template <typename T, typename... Args>
		T& AddSystem(Args&&... args)
		{
			systems[typeid(T).name()] = new T(std::forward<Args>(args)...);
			systems[typeid(T).name()]->scene = this;
			systems[typeid(T).name()]->Init();
			return *((T*)systems[typeid(T).name()]);
		}

		template <typename T>
		T& GetSystem()
		{
			return *((T*)systems[typeid(T).name()]);
		}

		template <typename T>
		bool HasSystem()
		{
			return systems.count(typeid(T).name()) > 0;
		}

		template <typename T>
		void DeleteSystem()
		{
			delete systems[typeid(T).name()];
			systems.erase(typeid(T).name());
		}


		friend class Entity;
	};
}