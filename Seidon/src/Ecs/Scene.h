#pragma once

#include "Core/UUID.h"
#include "System.h"

#include <unordered_map>
#include <string>
#include <typeinfo>
#include <iostream>

#include <entt/entt.hpp>


namespace Seidon
{
	class Entity;

	class Scene
	{
	private:
		UUID id;
		std::string name;
		entt::registry registry;
		std::unordered_map<std::string, System*> systems;

	public:
		Scene(const std::string& name = "Scene");
		~Scene();

		void Init();
		void Update(float deltaTime);
		void Destroy();

		void Save(std::ofstream& fileOut);
		void SaveText(std::ofstream& fileOut);

		void Load(std::ifstream& fileIn);
		void LoadText(std::string& path);

		Scene* Duplicate();
		void CopyEntities(Scene* other);
		void CopySystems(Scene* other);

		Entity CreateEntity(const std::string& name = std::string(), const UUID& id = UUID());
		void DestroyEntity(const Entity& entity);

		inline void SetName(const std::string& name) { this->name = name; }

		inline entt::registry& GetRegistry() { return registry; }
		inline const std::string& GetName() { return name; }

		template <typename T, typename... Args>
		T& AddSystem(Args&&... args)
		{
			systems[typeid(T).name()] = new T(std::forward<Args>(args)...);
			systems[typeid(T).name()]->scene = this;
			systems[typeid(T).name()]->SysInit();
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