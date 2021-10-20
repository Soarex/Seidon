#include "Scene.h"
#include "Entity.h"

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

	Entity Scene::CreateEntity(const std::string& name)
	{
		Entity e(registry.create(), &registry);
		e.AddComponent<TransformComponent>();

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
