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

#include "../Physics/PhysicsApi.h"
#include "../Audio/SoundApi.h"

#include <entt/entt.hpp>
#include <unordered_map>

namespace Seidon 
{
	class Application
	{
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
		inline PhysicsApi* GetPhysicsApi() { return physicsApi; }
		inline SoundApi* GetSoundApi() { return soundApi; }

		static inline Application* Get() { return instance; }

	public:
		template<typename Type>
		ComponentMetaType& RegisterComponent()
		{
			ComponentMetaType t;
			t.name = typeid(Type).name();
			t.Get = (void* (*)(Entity)) &Application::GetComponent<Type>;
			t.Add = (void* (*)(Entity)) &Application::AddComponent<Type>;
			t.Remove = &Application::RemoveComponent<Type>;
			t.Has = &Application::HasComponent<Type>;
			t.Copy = &Application::CopyComponent<Type>;

			registeredComponents.push_back(t);
			registeredComponentsIndexToName[typeid(Type).name()] = registeredComponents.size() - 1;

			return registeredComponents.back();
		}

		template<typename Type>
		void UnregisterComponent()
		{
			registeredComponents.erase(registeredComponents.begin() + registeredComponentsIndexToName[typeid(Type).name()]);
			registeredComponentsIndexToName.erase(typeid(Type).name());

			for (int i = 0; i < registeredComponents.size(); i++)
				registeredComponentsIndexToName[registeredComponents[i].name] = i;
		}

		template<typename Type>
		ComponentMetaType GetComponentMetaType()
		{
			return registeredComponents[registeredComponentsIndexToName.at(typeid(Type).name())];
		}
		
		ComponentMetaType GetComponentMetaTypeByName(const std::string& name)
		{
			return registeredComponents[registeredComponentsIndexToName.at(name)];
		}


		std::vector<ComponentMetaType> GetComponentMetaTypes()
		{
			return registeredComponents;
		}

		template<typename Type>
		bool IsComponentRegistered()
		{
			return registeredComponentsIndexToName.count(typeid(Type).name()) > 0;
		}

		bool IsComponentRegistered(const std::string& name)
		{
			return registeredComponentsIndexToName.count(name) > 0;
		}

		template<typename Type>
		SystemMetaType GetSystemMetaType()
		{
			return registeredSystems[registeredSystemsIndexToName.at(typeid(Type).name())];
		}

		SystemMetaType GetSystemMetaTypeByName(const std::string& name)
		{
			return registeredSystems[registeredSystemsIndexToName.at(name)];
		}


		std::vector<SystemMetaType> GetSystemsMetaTypes()
		{		
			return registeredSystems;
		}

		template<typename Type>
		bool IsSystemRegistered()
		{
			return registeredSystemsIndexToName.count(typeid(Type).name()) > 0;
		}

		bool IsSystemRegistered(const std::string& name)
		{
			return registeredSystemsIndexToName.count(name) > 0;
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


			registeredSystems.push_back(t);
			registeredSystemsIndexToName[typeid(Type).name()] = registeredSystems.size() - 1;

			return registeredSystems.back();
		}

		template<typename Type>
		void UnregisterSystem()
		{
			registeredSystems.erase(registeredSystems.begin() + registeredSystemsIndexToName[typeid(Type).name()]);
			registeredSystemsIndexToName.erase(typeid(Type).name());

			for (int i = 0; i < registeredSystems.size(); i++)
				registeredSystemsIndexToName[registeredSystems[i].name] = i;
		}

	protected:
		Window* window;

		SceneManager* sceneManager;
		InputManager* inputManager;
		ResourceManager* resourceManager;
		WorkManager* workManager;
		PhysicsApi* physicsApi;
		SoundApi* soundApi;

	public:
		std::vector<SystemMetaType> registeredSystems;
		std::unordered_map<std::string, int> registeredSystemsIndexToName;

		std::vector<ComponentMetaType> registeredComponents;
		std::unordered_map<std::string, int> registeredComponentsIndexToName;

		static Application* instance;

	private:
		template<typename Type>
		static Type& GetComponent(Entity entity)
		{
			return entity.GetComponent<Type>();
		}

		template<typename Type>
		static void* AddComponent(Entity entity)
		{
			return &entity.AddComponent<Type>();
		}

		template<typename Type>
		static void RemoveComponent(Entity entity)
		{
			return entity.RemoveComponent<Type>();
		}

		template<typename Type>
		static bool HasComponent(Entity entity)
		{
			return entity.HasComponent<Type>();
		}

		template<typename Type>
		static void CopyComponent(Entity src, Entity dst)
		{
			auto& component = src.GetComponent<Type>();
			dst.AddComponent<Type>(component);
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
			dst.AddSystem<Type>(/*src.GetSystem<Type>()*/);
		}
	};

	Application* CreateApplication();
}
