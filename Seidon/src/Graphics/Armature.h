#pragma once
#include <glm/glm.hpp>

#include <vector>
#include <string>

namespace Seidon
{
	struct BoneData
	{
		std::string name;
		int id;
		int parentId;
		glm::mat4 inverseBindPoseMatrix;
	};

	struct Armature
	{
		std::vector<BoneData> bones;
	};
}