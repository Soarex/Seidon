#pragma once
#include "Entity.h"

#include <entt/entt.hpp>
#include <string>

namespace Seidon
{
	class Prefab
	{
	private:
		static entt::registry prefabRegistry;

		Entity referenceEntity = { entt::null, nullptr };
	public:
		~Prefab();
		void MakeFromEntity(Entity e);

		void Save(const std::string& path);
		void Load(const std::string& path);

		friend class Scene;
	};
}