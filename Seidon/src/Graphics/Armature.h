#pragma once
#include "../Core/UUID.h"

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
		UUID id;
		std::string name;
		std::vector<BoneData> bones;

		void Save(const std::string& path);
		void SaveAsync(const std::string& path);
		void Load(const std::string& path);
		void LoadAsync(const std::string& path);
	};
}