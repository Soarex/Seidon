#include "Prefab.h"

#include "../Core/Application.h"

#include <fstream>

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
	}

	void Prefab::Save(const std::string& path)
	{
		std::ofstream out(path, std::ios::out | std::ios::binary);

		referenceEntity.Save(out);
	}

	void Prefab::Load(const std::string& path)
	{
		std::ifstream in(path, std::ios::in | std::ios::binary);

		referenceEntity = prefabScene.CreateEntity();

		referenceEntity.Load(in);
	}
}