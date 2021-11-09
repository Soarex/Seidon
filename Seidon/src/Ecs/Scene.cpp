#include "Scene.h"
#include "Entity.h"
#include "../Core/Application.h"

namespace Seidon
{

	Scene::Scene(const std::string& name)
		: name(name)
	{

	}


	Scene::~Scene()
	{
		Destroy();
	}

	void Scene::Init()
	{
		for (auto& [typeName, system] : systems)
			system->Init();
	}

	void Scene::Update(float deltaTime)
	{
		for (auto& [typeName, system] : systems)
			system->Update(deltaTime);
	}

	void Scene::Destroy()
	{
		for (auto& [typeName, system] : systems)
		{
			system->Destroy();
			delete system;
		}

		systems.clear();
		registry.clear();
	}

	Scene* Scene::Duplicate()
	{
		Scene* newScene = new Scene();

		auto& srcRegistry = registry;
		auto& dstRegistry = newScene->registry;

		std::unordered_map<UUID, entt::entity> enttMap;

		auto idView = srcRegistry.view<IDComponent>();
		for (auto e : idView)
		{
			UUID uuid = srcRegistry.get<IDComponent>(e).ID;
			std::string& name = srcRegistry.get<NameComponent>(e).name;

			Entity newEntity = newScene->CreateEntity(name, uuid);
			enttMap[uuid] = newEntity.ID;
		}

		const std::vector<MetaType>& components = Application::Get()->GetComponentMetaTypes();
		for (auto& metaType : components)
			metaType.Copy( srcRegistry, dstRegistry, enttMap);

		return newScene;
	}

	Entity Scene::CreateEntity(const std::string& name, const UUID& id)
	{
		Entity e(registry.create(), &registry);
		e.AddComponent<TransformComponent>();
		e.AddComponent<IDComponent>();

		if (name == "")
			e.AddComponent<NameComponent>("Entity #" + std::to_string((int)e.ID));
		else
			e.AddComponent<NameComponent>(name);

		return e;
	}

	void Scene::DestroyEntity(const Entity& entity)
	{
		registry.destroy(entity.ID);
	}
}
