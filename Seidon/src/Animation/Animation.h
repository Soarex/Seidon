#pragma once
#include "../Core/UUID.h"

#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

#include <string>
#include <vector>

#define MAX_BONE_COUNT 150

namespace Seidon
{
	struct PositionKey
	{
		float time;
		glm::vec3 value;

		PositionKey()
		{
			time = 0;
			value = glm::vec3(0);
		}
	};
	
	struct RotationKey
	{
		float time;
		glm::quat value;

		RotationKey()
		{
			time = 0;
			value = glm::quat(1, 0, 0, 0);
		}
	};

	struct ScalingKey
	{
		float time;
		glm::vec3 value;

		ScalingKey()
		{
			time = 0;
			value = glm::vec3(1);
		}
	};

	struct AnimationChannel
	{
		std::string boneName;
		int boneId;
		std::vector<PositionKey> positionKeys;
		std::vector<RotationKey> rotationKeys;
		std::vector<ScalingKey> scalingKeys;
	};

	class Animation
	{
	public:
		UUID id;
		std::string name;
		float duration;
		float ticksPerSecond;
		
		std::vector<AnimationChannel> channels;
	public:
		Animation() = default;

		void Save(const std::string& path);
		void SaveAsync(const std::string& path);
		void Load(const std::string& path);
		void LoadAsync(const std::string& path);
	};
}