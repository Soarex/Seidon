#include "Scene.h"
#include "Entity.h"
#include "Prefab.h"
#include "../Core/Application.h"

#include <iostream>
#include <fstream>

namespace Seidon
{
	Scene::Scene(const std::string& name, UUID id)
	{
		this->name = name;
		this->id = id;
	}


	Scene::~Scene()
	{
		Destroy();
	}

	void Scene::Init()
	{
	}

	//LONG WINAPI ExceptionFilterFunction(struct _EXCEPTION_POINTERS* ExceptionInfo)
	//{
	//	std::cerr << "Exception during system execution" << std::endl;
	//	throw "Error";
	//}

	void Scene::Update(float deltaTime)
	{
		CreateViewAndIterate<TransformComponent>
		(
			[](EntityId id, TransformComponent& transform)
			{
				transform.cacheValid = false;
			}
		);

		//LPTOP_LEVEL_EXCEPTION_FILTER oldExceptionFilter = SetUnhandledExceptionFilter(&ExceptionFilterFunction);

		//try
		//{
			for (auto& [typeName, system] : systems)
				system->SysUpdate(deltaTime);
		//}
		//catch (...)
		//{
		//	std::cerr << "Caught" << std::endl;
		//}

		//SetUnhandledExceptionFilter(oldExceptionFilter);
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

			idToEntityMap[e.GetId()] = e.ID;
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
		e.AddComponent<TransformComponent>().parent = 0;
		UUID i = e.AddComponent<IDComponent>(id).ID;
		e.AddComponent<MouseSelectionComponent>();

		if (name == "")
			e.AddComponent<NameComponent>("Entity #" + std::to_string((int)e.ID));
		else
			e.AddComponent<NameComponent>(name);

		idToEntityMap[i] = e.ID;

		return e;
	}

	Entity Scene::InstantiatePrefab(Prefab& prefab, const std::string& name)
	{
		Entity e(registry.create(), this);

		const std::vector<ComponentMetaType>& components = Application::Get()->GetComponentMetaTypes();
		for (auto& metaType : components)
			if (metaType.Has(prefab.GetRootEntity()))
				metaType.Copy(prefab.GetRootEntity(), e);

		e.GetComponent<IDComponent>().ID = UUID();

		if (name != "")
			e.GetComponent<NameComponent>().name = name;

		TransformComponent& t = e.GetComponent<TransformComponent>();
		t.parent = 0;
		t.children.clear();

		for (UUID childId : prefab.GetRootEntity().GetChildrenIds())
			AddChildEntityFromPrefab(e, prefab.prefabScene.GetEntityById(childId));

		idToEntityMap[e.GetComponent<IDComponent>().ID] = e.ID;

		return e;
	}

	Entity Scene::InstantiatePrefab(Prefab& prefab, const glm::vec3& position, const glm::vec3& rotation, const glm::vec3& scale, const std::string& name)
	{
		Entity e(registry.create(), this);

		const std::vector<ComponentMetaType>& components = Application::Get()->GetComponentMetaTypes();
		for (auto& metaType : components)
			if (metaType.Has(prefab.GetRootEntity()))
				metaType.Copy(prefab.GetRootEntity(), e);

		e.GetComponent<IDComponent>().ID = UUID();

		if (name != "")
			e.GetComponent<NameComponent>().name = name;

		TransformComponent& t = e.GetComponent<TransformComponent>();
		t.position = position;
		t.rotation = rotation;
		t.scale = scale;
		t.parent = 0;
		t.children.clear();

		for (UUID childId : prefab.GetRootEntity().GetChildrenIds())
			AddChildEntityFromPrefab(e, prefab.prefabScene.GetEntityById(childId));

		idToEntityMap[e.GetComponent<IDComponent>().ID] = e.ID;
		
		return e;
	}

	void Scene::AddChildEntityFromPrefab(Entity parentEntity, Entity prefabEntity)
	{
		Entity e(registry.create(), this);

		const std::vector<ComponentMetaType>& components = Application::Get()->GetComponentMetaTypes();
		for (auto& metaType : components)
			if (metaType.Has(prefabEntity))
				metaType.Copy(prefabEntity, e);

		e.GetComponent<IDComponent>().ID = UUID();

		TransformComponent& t = e.GetComponent<TransformComponent>();
		t.parent = parentEntity.GetId();
		t.children.clear();

		parentEntity.AddChild(e);

		for (UUID childId : prefabEntity.GetChildrenIds())
			AddChildEntityFromPrefab(e, prefabEntity.scene->GetEntityById(childId));

		idToEntityMap[e.GetComponent<IDComponent>().ID] = e.ID;
	}

	void Scene::DestroyEntity(Entity& entity)
	{
		if (entity.HasParent())
			entity.GetParent().RemoveChild(entity);

		for (UUID id : entity.GetChildrenIds())
		{
			Entity e = GetEntityById(id);
			DestroyEntity(e);
		}

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
