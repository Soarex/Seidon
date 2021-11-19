#pragma once

#include "../Reflection/Reflection.h"

#include "Window.h"
#include "InputManager.h"
#include "ResourceManager.h"
#include "WorkManager.h"

#include "Ecs/Components.h"
#include "Ecs/SceneManager.h"
#include "Ecs/Scene.h"
#include "Ecs/Entity.h"

#include <entt/entt.hpp>
#include <unordered_map>

namespace Seidon 
{
	class Application
	{
	public:
		std::unordered_map<std::string, SystemMetaType> registeredSystems;
		std::vector<ComponentMetaType> registeredComponents;

		static Application* instance;
	protected:
		Window* window;

		SceneManager* sceneManager;
		InputManager* inputManager;
		ResourceManager* resourceManager;
		WorkManager* workManager;
	public:
		Application();
		virtual ~Application();

		void AppInit();
		void AppUpdate();
		void AppDestroy();

		virtual void Init();
		virtual void Update();
		virtual void Destroy();

		Window* GetWindow() { return window; }

		inline InputManager* GetInputManager() { return inputManager; }
		inline ResourceManager* GetResourceManager() { return resourceManager; }
		inline WorkManager* GetWorkManager() { return workManager; }
		inline SceneManager* GetSceneManager() { return sceneManager; }

		static inline Application* Get() { return instance; }

	private:
		template<typename Type>
		static Type& GetComponent(Entity& entity)
		{
			return entity.GetComponent<Type>();
		}

		template<typename Type>
		static Type& AddComponent(Entity& entity)
		{
			return entity.AddComponent<Type>();
		}

		template<typename Type>
		static void RemoveComponent(Entity& entity)
		{
			return entity.RemoveComponent<Type>();
		}

		template<typename Type>
		static bool HasComponent(Entity& entity)
		{
			return entity.HasComponent<Type>();
		}

		template<typename Type>
		static void CopyComponent(entt::registry& src, entt::registry& dst, const std::unordered_map<UUID, entt::entity>& enttMap)
		{
			auto view = src.view<Type>();
			for (auto e : view)
			{
				UUID uuid = src.get<IDComponent>(e).ID;
				entt::entity dstEnttID = enttMap.at(uuid);

				Type& component = src.get<Type>(e);
				dst.emplace_or_replace<Type>(dstEnttID, component);
			}
		}

		template<typename Type>
		static Type& AddSystem(Scene& scene)
		{
			return scene.AddSystem<Type>();
		}

		template<typename Type>
		static bool HasSystem(Scene& scene)
		{
			return scene.HasSystem<Type>();
		}

		template<typename Type>
		static Type& GetSystem(Scene& scene)
		{
			return scene.GetSystem<Type>();
		}

		template<typename Type>
		static void DeleteSystem(Scene& scene)
		{
			scene.DeleteSystem<Type>();
		}

		template<typename Type>
		static void CopySystem(Scene& src, Scene& dst)
		{
			dst.AddSystem<Type>(src.GetSystem<Type>());
		}

	public:
		template<typename Type>
		ComponentMetaType& RegisterComponent()
		{
			ComponentMetaType t;
			t.name = typeid(Type).name();
			t.Get = (void* (*)(Entity&)) &Application::GetComponent<Type>;
			t.Add = (void* (*)(Entity&)) &Application::AddComponent<Type>;
			t.Remove = &Application::RemoveComponent<Type>;
			t.Has = &Application::HasComponent<Type>;
			t.Copy = &Application::CopyComponent<Type>;

			registeredComponents.push_back(t);
			return registeredComponents.back();
		}

		std::vector<ComponentMetaType> GetComponentMetaTypes()
		{
			return registeredComponents;
		}

		std::vector<SystemMetaType> GetSystemsMetaTypes()
		{
			std::vector<SystemMetaType> res;

			for (auto& [name, system] : registeredSystems)
				res.push_back(system);
			
			return res;
		}

		template<typename Type>
		SystemMetaType& RegisterSystem()
		{
			SystemMetaType t;

			t.name = typeid(Type).name();
			t.Get = (void* (*)(Scene&)) & Application::GetSystem<Type>;
			t.Delete = (void* (*)(Scene&)) & Application::DeleteSystem<Type>;
			t.Add = (void* (*)(Scene&)) & Application::AddSystem<Type>;
			t.Has = &Application::HasSystem<Type>;
			t.Copy = &Application::CopySystem<Type>;

			registeredSystems[typeid(Type).name()] = t;
			return registeredSystems[typeid(Type).name()];
		}

		template<typename Type>
		void UnregisterSystem()
		{
			registeredSystems.erase(typeid(Type).name());
		}
	};

	Application* CreateApplication();
}
