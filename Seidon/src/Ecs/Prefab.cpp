#include "Prefab.h"

#include "../Core/Application.h"

#include <fstream>
#include <iostream>
#include <yaml-cpp/yaml.h>

namespace Seidon
{
	Scene Prefab::prefabScene;

	Prefab::~Prefab()
	{
		if (referenceEntity.ID != entt::null)
			prefabScene.DestroyEntity(referenceEntity);
	}

	void Prefab::MakeFromEntity(Entity e)
	{
		if(referenceEntity.ID != entt::null)
			prefabScene.DestroyEntity(referenceEntity);

		referenceEntity = prefabScene.CreateEntity();

		referenceEntity.RemoveComponent<NameComponent>();
		referenceEntity.RemoveComponent<IDComponent>();
		referenceEntity.RemoveComponent<MouseSelectionComponent>();
		referenceEntity.RemoveComponent<TransformComponent>();

		const std::vector<ComponentMetaType>& components = Application::Get()->GetComponentMetaTypes();
		for (auto& metaType : components)
			if (metaType.Has(e))
				metaType.Copy(e, referenceEntity);

		referenceEntity.RemoveComponent<IDComponent>();
	}

	void Prefab::Save(const std::string& path)
	{
		std::ofstream fileOut(path);

		YAML::Emitter out;

		referenceEntity.SaveText(out);

		fileOut << out.c_str();
	}

	void Prefab::Load(const std::string& path)
	{
		YAML::Node data;

		try
		{
			data = YAML::LoadFile(path);
		}
		catch (YAML::ParserException e)
		{
			std::cout << e.msg << std::endl;
			return;
		}

		referenceEntity = prefabScene.CreateEntity();

		referenceEntity.LoadText(data);
	}
}