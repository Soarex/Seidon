#include "Prefab.h"

#include "../Core/Application.h"

#include <fstream>

namespace Seidon
{
	void Prefab::MakeFromEntity(Entity entity)
	{
		prefabScene.Destroy();

		rootEntityId = AddEntityHierarchy(entity).GetId();
	}

	Entity Prefab::AddEntityHierarchy(Entity entity)
	{
		Entity e = prefabScene.CreateEntity();

		const std::vector<ComponentMetaType>& components = Application::Get()->GetComponentMetaTypes();
		for (auto& metaType : components)
			if (metaType.Has(entity))
				metaType.Copy(entity, e);

		for (UUID childId : entity.GetChildrenIds())
			AddEntityHierarchy(entity.scene->GetEntityById(childId));

		return e;
	}

	Entity Prefab::GetRootEntity()
	{
		return prefabScene.GetEntityById(rootEntityId);
	}

	void Prefab::Save(const std::string& path)
	{
		std::ofstream out(path, std::ios::out | std::ios::binary);

		out.write((char*)&rootEntityId, sizeof(UUID));
		prefabScene.Save(out);
	}

	void Prefab::Load(const std::string& path)
	{
		std::ifstream in(path, std::ios::in | std::ios::binary);

		in.read((char*)&rootEntityId, sizeof(UUID));
		prefabScene.Load(in);
	}
}