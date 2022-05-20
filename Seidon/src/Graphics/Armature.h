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

		Armature(UUID id = UUID());

		void Save(std::ofstream& out);
		void SaveAsync(std::ofstream& out);
		void Load(std::ifstream& in);
		void LoadAsync(std::ifstream& in);
	};
}