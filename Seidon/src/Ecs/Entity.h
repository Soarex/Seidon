#pragma once
#include "../Core/UUID.h"

#include "Components.h"
#include "entt/entt.hpp"

#include <iostream>

namespace Seidon
{
	class Entity
	{
	public:
		entt::entity ID;
		entt::registry* registry;
	public:
		Entity() = default;
		Entity(const Entity& entity) = default;
		Entity(entt::entity id, entt::registry* registry);

		void Save(std::ofstream& fileOut);
		void SaveText(std::ofstream& fileOut);

		void Load(std::ifstream& fileIn);
		void LoadText(const std::string& code);

		template <typename T, typename... Args>
		T& AddComponent(Args&&... args)
		{
			return registry->emplace<T>(ID, std::forward<Args>(args)...);
		}

		template <typename T>
		void EditComponent(std::function<void(T&)> editFunction)
		{
			registry->patch<T>(ID, editFunction);
		}

		template <typename T>
		void RemoveComponent()
		{
			registry->remove<T>(ID);
		}

		template <typename T>
		T& GetComponent()
		{
			return registry->get<T>(ID);
		}

		template <typename T>
		bool HasComponent()
		{
			return registry->all_of<T>(ID);
		}

		bool operator ==(const Entity& other) { return ID == other.ID; }
	};
}