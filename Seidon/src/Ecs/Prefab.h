#pragma once
#include "Entity.h"

#include <entt/entt.hpp>
#include <string>

namespace Seidon
{
	class Prefab
	{
	private:
		UUID rootEntityId;
		Scene prefabScene;
	public:
		void MakeFromEntity(Entity entity);
		Entity GetRootEntity();

		void Save(const std::string& path);
		void Load(const std::string& path);

	private:
		Entity AddEntityHierarchy(Entity entity);

		friend class Scene;
	};
}