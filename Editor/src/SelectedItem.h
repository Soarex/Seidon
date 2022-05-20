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
		BoneData* bone;
		glm::mat4* transform;
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