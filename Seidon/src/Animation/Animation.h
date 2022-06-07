#pragma once
#include "../Core/UUID.h"
#include "../Core/Asset.h"

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

	class Animation : public Asset
	{
	public:
		float duration;
		float ticksPerSecond;
		
		std::vector<AnimationChannel> channels;

	public:
		Animation(UUID id = UUID());

		using Asset::Save;
		using Asset::Load;

		void Save(std::ofstream& out) override;
		void Load(std::ifstream& in) override;
	};
}