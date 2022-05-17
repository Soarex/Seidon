#include "Scene.h"
#include "Entity.h"
#include "Prefab.h"
#include "../Core/Application.h"

#include <iostream>
#include <fstream>

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
	}

	void Scene::Update(float deltaTime)
	{
		CreateViewAndIterate<TransformComponent>
		(
			[](EntityId id, TransformComponent& transform)
			{
				transform.cacheValid = false;
			}
		);

		for (auto& [typeName, system] : systems)
			system->SysUpdate(deltaTime);
	}

	void Scene::Save(std::ofstream& out)
	{
		out.write((char*)&id, sizeof(UUID));

		size_t size = name.length() + 1;

		out.write((char*)&size, sizeof(size_t));
		out.write(name.c_str(), size * sizeof(char));

		size_t entityCount = registry.alive();
		out.write((char*)&entityCount, sizeof(size_t));

		registry.each([&](auto entityID)
		{
			Entity entity = { entityID, this };
			entity.Save(out);
		});

		size_t systemCount = systems.size();
		out.write((char*)&systemCount, sizeof(size_t));

		for (auto& [name, system] : systems)
		{
			SystemMetaType metaType = Application::Get()->GetSystemMetaTypeByName(name);

			size_t size = metaType.name.length() + 1;

			out.write((char*)&size, sizeof(size_t));
			out.write(metaType.name.c_str(), size * sizeof(char));

			size_t memberCount = metaType.members.size();
			out.write((char*)&memberCount, sizeof(size_t));

			metaType.Save(out, (byte*)system);
		}
	}

	void Scene::Load(std::ifstream& in)
	{
		in.read((char*)&id, sizeof(UUID));

		size_t size = 0;
		char buffer[500];

		in.read((char*)&size, sizeof(size_t));
		in.read(buffer, size * sizeof(char));

		name = std::string(buffer);

		size_t entityCount = 0;
		in.read((char*)&entityCount, sizeof(size_t));

		for (int i = 0; i < entityCount; i++)
		{
			Entity e(registry.create(), this);
			e.Load(in);
		}

		size_t systemCount = 0;
		in.read((char*)&systemCount, sizeof(size_t));

		for (int i = 0; i < systemCount; i++)
		{
			size_t size = 0;

			in.read((char*)&size, sizeof(size_t));
			in.read(buffer, size * sizeof(char));

			if (!Application::Get()->IsSystemRegistered(buffer))
			{
				std::cerr << "Serialized system '" << buffer << "' is not a registered system" << std::endl;
				continue;
			}

			SystemMetaType metaType = Application::Get()->GetSystemMetaTypeByName(buffer);

			size_t memberCount = 0;
			in.read((char*)&memberCount, sizeof(size_t));

			if (metaType.members.size() != memberCount)
			{
				std::cerr << "Serialized system '" << buffer << "' not compatible with current types" << std::endl;
				continue;
			}

			void* system = metaType.Add(*this);
			metaType.Load(in, (byte*)system);
		}
	}

	void Scene::Destroy()
	{
		for (auto& [typeName, system] : systems)
		{
			system->SysDestroy();
			delete system;
		}

		systems.clear();
		registry.clear();
	}

	Scene* Scene::Duplicate()
	{
		Scene* newScene = new Scene();

		CopyEntities(newScene);
		CopySystems(newScene);

		return newScene;
	}

	void Scene::CopyEntities(Scene* other)
	{
		auto& srcRegistry = registry;
		auto& dstRegistry = other->registry;
		std::unordered_map<UUID, EntityId> enttMap;

		auto idView = srcRegistry.view<IDComponent>();
		for (auto e : idView)
		{
			UUID uuid = srcRegistry.get<IDComponent>(e).ID;
			std::string& name = srcRegistry.get<NameComponent>(e).name;

			Entity newEntity = other->CreateEntity(name, uuid);
			enttMap[uuid] = newEntity.ID;
		}

		const std::vector<ComponentMetaType>& components = Application::Get()->GetComponentMetaTypes();
		srcRegistry.each([&](auto entityId) 
			{
				for (auto& metaType : components)
					if (metaType.Has(Entity(entityId, this)))
					{
						UUID uuid = srcRegistry.get<IDComponent>(entityId).ID;
						EntityId dstEnttID = enttMap.at(uuid);

						metaType.Copy(Entity(entityId, this), Entity(dstEnttID, other));
					}
			});

	}

	void Scene::CopySystems(Scene* other)
	{
		const std::vector<SystemMetaType>& systems = Application::Get()->GetSystemsMetaTypes();
		for (auto& metaType : systems)
			if (metaType.Has(*this))
			{
				//metaType.Copy(*this, *other);
				metaType.Add(*other);
			}
	}

	Entity Scene::CreateEntity(const std::string& name, const UUID& id)
	{
		Entity e(registry.create(), this);
		e.AddComponent<TransformComponent>();
		UUID i = e.AddComponent<IDComponent>(id).ID;
		e.AddComponent<MouseSelectionComponent>();

		if (name == "")
			e.AddComponent<NameComponent>("Entity #" + std::to_string((int)e.ID));
		else
			e.AddComponent<NameComponent>(name);

		idToEntityMap[i] = e.ID;

		return e;
	}

	Entity Scene::CreateEntityFromPrefab(Prefab& prefab, const std::string& name, const UUID& id)
	{
		Entity e(registry.create(), this);

		const std::vector<ComponentMetaType>& components = Application::Get()->GetComponentMetaTypes();
		for (auto& metaType : components)
			if (metaType.Has(prefab.referenceEntity))
				metaType.Copy(prefab.referenceEntity, e);

		e.GetComponent<IDComponent>().ID = id;
		e.AddComponent<MouseSelectionComponent>();

		if (name != "")
			e.GetComponent<NameComponent>().name = name;

		idToEntityMap[id] = e.ID;
		
		return e;
	}

	void Scene::DestroyEntity(Entity& entity)
	{
		idToEntityMap.erase(entity.GetId());

		const std::vector<ComponentMetaType>& components = Application::Get()->GetComponentMetaTypes();
		for (auto& metaType : components)
			if (metaType.Has(entity))
				metaType.Remove(entity);

		registry.destroy(entity.ID);
	}

	Entity Scene::GetEntityByEntityId(EntityId id)
	{ 
		return Entity(id, this); 
	}

	Entity Scene::GetEntityById(UUID id)
	{
		return Entity(idToEntityMap.at(id), this);
	}
}
