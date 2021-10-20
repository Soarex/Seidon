#pragma once

#include "Window.h"
#include "InputManager.h"
#include "ResourceManager.h"
#include "WorkManager.h"
#include "Ecs/SceneManager.h"
#include "Ecs/Scene.h"
#include "Ecs/EcsContext.h"

#include <entt/entt.hpp>
#include <unordered_map>

namespace Seidon 
{
	class Application
	{
	public:
		EcsContext* ecsContext;
		std::unordered_map<std::string, void(*)(Scene& scene)> registeredSystems;

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
		inline EcsContext* GetEcsContext() { return ecsContext; }

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
		static void AddSystem(Scene& scene)
		{
			scene.AddSystem<Type>();
		}

	public:
		template<typename Type>
		void RegisterComponent()
		{
			entt::meta<Type>()
				.type(entt::type_id<Type>().hash())
				.func<&Application::GetComponent<Type>>(entt::hashed_string("GetComponent"))
				.func<&Application::AddComponent<Type>>(entt::hashed_string("AddComponent"))
				.func<&Application::HasComponent<Type>>(entt::hashed_string("HasComponent"));
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
