#pragma once

#include <entt/entt.hpp>

namespace Seidon
{
	class Entity;
	class EntityManager
	{
	public:
		static entt::registry registry;
	public:
		static void Init();
		static Entity CreateEntity(const std::string& name = std::string());
		static void DestroyEntity(const Entity& entity);

		friend class Entity;
	};
}