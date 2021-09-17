#include "EntityManager.h"
#include "Entity.h"

namespace Seidon
{
	entt::registry EntityManager::registry;

	void EntityManager::Init()
	{

	}

	Entity EntityManager::CreateEntity(const std::string& name)
	{
		Entity e(registry.create(), &registry);
		e.AddComponent<TransformComponent>();

		if (name == "")
			e.AddComponent<NameComponent>("Entity #" + std::to_string((int)e.ID));
		else
			e.AddComponent<NameComponent>(name);

		return e;
	}

	void EntityManager::DestroyEntity(const Entity& entity)
	{
		registry.destroy(entity.ID);
	}
}