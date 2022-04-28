#include "Prefab.h"

#include "../Core/Application.h"

#include <fstream>
#include <iostream>
#include <yaml-cpp/yaml.h>

namespace Seidon
{
	entt::registry Prefab::prefabRegistry;

	Prefab::~Prefab()
	{
		if (referenceEntity.ID != entt::null)
			prefabRegistry.destroy(referenceEntity.ID);
	}

	void Prefab::MakeFromEntity(Entity e)
	{
		if(referenceEntity.ID != entt::null)
			prefabRegistry.destroy(referenceEntity.ID);

		referenceEntity = Entity(prefabRegistry.create(), &prefabRegistry);

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

		referenceEntity = Entity(prefabRegistry.create(), &prefabRegistry);

		referenceEntity.LoadText(data);
	}
}