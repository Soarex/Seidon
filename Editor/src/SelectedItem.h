#pragma once
#include <Seidon.h>

namespace Seidon
{
	enum class SelectedItemType
	{
		NONE = 0,
		ENTITY,
		MATERIAL,
		BONE
	};

	struct BoneSelectionData
	{
		int id;
		Entity owningEntity;
		Armature* armature;
		std::vector<glm::mat4>* transforms;
	};

	struct SelectedItem
	{
		SelectedItemType type = SelectedItemType::NONE;

		union
		{
			BoneSelectionData boneData;
			Entity entity;
			Material* material;
		};
	};
}