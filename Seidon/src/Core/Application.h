#pragma once

#include "../Reflection/Reflection.h"

#include "Window.h"
#include "InputManager.h"
#include "ResourceManager.h"
#include "WorkManager.h"

#include "Ecs/Components.h"
#include "Ecs/SceneManager.h"
#include "Ecs/Scene.h"

#include <entt/entt.hpp>
#include <unordered_map>

namespace Seidon 
{
	class Application
	{
	public:
		std::unordered_map<std::string, void(*)(Scene& scene)> registeredSystems;
		std::vector<MetaType> registeredComponents;

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
		static Type& GetComponent(entt::registry& registry, entt::entity entity)
		{
			return registry.get<Type>(entity);
		}

		template<typename Type>
		static Type& AddComponent(entt::registry& registry, entt::entity entity)
		{
			return registry.emplace<Type>(entity);
		}

		template<typename Type>
		static bool HasComponent(entt::registry& registry, entt::entity entity)
		{
			return registry.all_of<Type>(entity);
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
		static void AddSystem(Scene& scene)
		{
			scene.AddSystem<Type>();
		}

	public:
		template<typename Type>
		MetaType& RegisterComponent()
		{
			MetaType t;
			t.name = typeid(Type).name();
			t.Get	= (void*(*)(entt::registry &,entt::entity))&Application::GetComponent<Type>;
			t.Add	= (void*(*)(entt::registry &,entt::entity))&Application::AddComponent<Type>;
			t.Has	= &Application::HasComponent<Type>;
			t.Copy	= &Application::CopyComponent<Type>;

			registeredComponents.push_back(t);
			return registeredComponents.back();
		}

		std::vector<MetaType> GetComponentMetaTypes()
		{

			return registeredComponents;
		}

		template<typename Type>
		void RegisterSystem()
		{
			registeredSystems[typeid(Type).name()] = &Application::AddSystem<Type>;
		}

		template<typename Type>
		void UnregisterSystem()
		{
			registeredSystems.erase(typeid(Type).name());
		}
	};

	Application* CreateApplication();
}
