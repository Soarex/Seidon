#pragma once

#include <glm/glm.hpp>

#include <string>
#include <vector>

namespace Seidon
{
	struct AnimationKey
	{
		float time;
		glm::vec4 value;
	};

	struct AnimationChannel
	{
		std::string boneName;
		std::vector<AnimationKey> positionKeys;
		std::vector<AnimationKey> rotationKeys;
		std::vector<AnimationKey> scalingKeys;
	};

	class Animation
	{
	public:
		std::string name;
		float duration;
		float ticksPerSecond;
		
		std::vector<AnimationChannel> channels;
	};
}