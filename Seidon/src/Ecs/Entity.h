#pragma once
#include "../Core/UUID.h"

#include "Scene.h"
#include "Components.h"
#include "entt/entt.hpp"

#include <yaml-cpp/yaml.h>

#include <iostream>

namespace Seidon
{
	class Entity
	{
	public:
		entt::entity ID;
		Scene* scene;
		
		//entt::registry* registry;
	public:
		Entity() = default;
		Entity(const Entity& entity) = default;
		Entity(entt::entity id, Scene* scene);

		void Save(std::ofstream& fileOut);
		void SaveText(YAML::Emitter& out);

		void Load(std::ifstream& fileIn);
		void LoadText(YAML::Node& entityNode);

		Entity Duplicate();

		template <typename T, typename... Args>
		T& AddComponent(Args&&... args)
		{
			T& component = scene->GetRegistry().emplace_or_replace<T>(ID, std::forward<Args>(args)...);
			scene->OnComponentAdded<T>(ID);
			return component;
		}

		template <typename T>
		void EditComponent(std::function<void(T&)> editFunction)
		{
			scene->GetRegistry().patch<T>(ID, editFunction);
		}

		template <typename T>
		void RemoveComponent()
		{
			scene->OnComponentRemoved<T>(ID);
			scene->GetRegistry().remove<T>(ID);
		}

		template <typename T>
		T& GetComponent()
		{
			return scene->GetRegistry().get<T>(ID);
		}

		template <typename T>
		bool HasComponent()
		{
			return scene->GetRegistry().all_of<T>(ID);
		}

		bool IsValid()
		{
			return scene->GetRegistry().valid(ID);
		}

		bool operator ==(const Entity& other) { return ID == other.ID; }
	};
}