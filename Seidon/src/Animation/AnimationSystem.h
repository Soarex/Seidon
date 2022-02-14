#pragma once
#include "../Ecs/System.h"

#include "Animation.h"
#include <vector>
#include <glm/glm.hpp>

namespace Seidon
{
	class AnimationSystem : public System
	{
	private:
		glm::mat4 localBoneMatrices[100];

		float lastPositionKeyIndices[100];
		float lastRotationKeyIndices[100];
		float lastScalingKeyIndices[100];
	public:
		AnimationSystem() = default;
		~AnimationSystem() = default;

		void Init() override;
		void Update(float deltatime) override;
		void Destroy() override;

	private:
		int GetPositionKeyIndex(std::vector<PositionKey>& keys, float currentTime, int lastPosition);
		int GetRotationKeyIndex(std::vector<RotationKey>& keys, float currentTime, int lastPosition);
		int GetScalingKeyIndex(std::vector<ScalingKey>& keys, float currentTime, int lastPosition);
	};
}