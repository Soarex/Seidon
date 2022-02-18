#include "AnimationSystem.h"

#include "Ecs/Scene.h"
#include "Ecs/Components.h"

#include <glm/gtx/quaternion.hpp>
#include <iostream>

namespace Seidon
{
	void AnimationSystem::Init()
	{
	}

	void AnimationSystem::Update(float deltaTime)
	{
		entt::basic_group group = scene->GetRegistry().group<AnimationComponent>(entt::get<SkinnedRenderComponent, TransformComponent>);

		for (entt::entity e : group)
		{
			AnimationComponent& a = group.get<AnimationComponent>(e);
			SkinnedRenderComponent& r = group.get<SkinnedRenderComponent>(e);
			TransformComponent& t = group.get<TransformComponent>(e);
			a.runtimeBoneMatrices.clear();

			Animation* animation = a.animation;
			Armature* armature = r.armature;

			if (animation->channels.size() != armature->bones.size())
			{
				std::cout << animation->channels.size() << " " << armature->bones.size() << std::endl;
				std::cerr << "Animation " << animation->name << " incompatible with armature " << armature->name << std::endl;
				return;
			}

			float duration = animation->duration * animation->ticksPerSecond;

			a.runtimeTime += animation->ticksPerSecond * deltaTime;
			float timePoint = fmod(a.runtimeTime, animation->duration);

			int channelIndex = 0;
			for (int i = 0; i < armature->bones.size(); i++)
			{
				BoneData& bone = armature->bones[i];

				glm::mat4 parentTransform = glm::identity<glm::mat4>();

				if (i > 0)
					parentTransform = a.localBoneMatrices[bone.parentId];

				AnimationChannel& channel = animation->channels[i];

				int keyIndex = GetPositionKeyIndex(channel.positionKeys, timePoint, a.lastPositionKeyIndices[i]);

				glm::mat4 translation;
				if (channel.positionKeys.size() > 1)
				{
					float currentKeyTime = channel.positionKeys[keyIndex].time;
					float nextKeyTime = channel.positionKeys[keyIndex + 1].time;

					translation = glm::translate(
						glm::identity<glm::mat4>(),
						glm::mix(channel.positionKeys[keyIndex].value, channel.positionKeys[keyIndex + 1].value, (timePoint - currentKeyTime) / (nextKeyTime - currentKeyTime))
					);
				}
				else
					translation = glm::translate(glm::identity<glm::mat4>(), channel.positionKeys[keyIndex].value);

				a.lastPositionKeyIndices[i] = keyIndex;

				keyIndex = GetRotationKeyIndex(channel.rotationKeys, timePoint, a.lastRotationKeyIndices[i]);

				glm::mat4 rotation;
				if (channel.rotationKeys.size() > 1)
				{
					float currentKeyTime = channel.rotationKeys[keyIndex].time;
					float nextKeyTime = channel.rotationKeys[keyIndex + 1].time;

					 rotation = glm::toMat4(
						glm::slerp(channel.rotationKeys[keyIndex].value, channel.rotationKeys[keyIndex + 1].value, (timePoint - currentKeyTime) / (nextKeyTime - currentKeyTime))
					);
				}
				else
					rotation = glm::toMat4(channel.rotationKeys[keyIndex].value);

				a.lastRotationKeyIndices[i] = keyIndex;

				keyIndex = GetScalingKeyIndex(channel.scalingKeys, timePoint, a.lastScalingKeyIndices[i]);

				glm::mat4 scale;
				if (channel.scalingKeys.size() > 1)
				{
					float currentKeyTime = channel.scalingKeys[keyIndex].time;
					float nextKeyTime = channel.scalingKeys[keyIndex + 1].time;

					scale = glm::scale(
						glm::identity<glm::mat4>(),
						glm::mix(channel.scalingKeys[keyIndex].value, channel.scalingKeys[keyIndex + 1].value, (timePoint - currentKeyTime) / (nextKeyTime - currentKeyTime))
					);
				}
				else
					scale = glm::scale(glm::identity<glm::mat4>(), channel.scalingKeys[keyIndex].value);

				a.lastScalingKeyIndices[i] = keyIndex;

				glm::mat4 animationTransform = translation * rotation * scale;

				a.localBoneMatrices[i] = parentTransform * animationTransform;

				a.runtimeBoneMatrices.push_back(a.localBoneMatrices[i] * bone.inverseBindPoseMatrix);
			}
		}
	}

	void AnimationSystem::Destroy()
	{

	}

	int AnimationSystem::GetPositionKeyIndex(std::vector<PositionKey>& keys, float currentTime, int lastPosition)
	{
		if (keys.size() == 1) return 0;

		int res = lastPosition;

		do
		{
			if (currentTime > keys[res].time && currentTime < keys[res + 1].time)
			{
				lastPosition = res;
				return lastPosition;
			}

			res++;
			if (res == keys.size() - 1) res = 0;
		} while (res != lastPosition);

		return 0;
	}

	int AnimationSystem::GetRotationKeyIndex(std::vector<RotationKey>& keys, float currentTime, int lastPosition)
	{
		if (keys.size() == 1) return 0;

		int res = lastPosition;

		do
		{
			if (currentTime > keys[res].time && currentTime < keys[res + 1].time)
			{
				lastPosition = res;
				return lastPosition;
			}

			res++;
			if (res == keys.size() - 1) res = 0;
		} while (res != lastPosition);

		return 0;
	}

	int AnimationSystem::GetScalingKeyIndex(std::vector<ScalingKey>& keys, float currentTime, int lastPosition)
	{
		if (keys.size() == 1) return 0;

		int res = lastPosition;

		do
		{
			if (currentTime > keys[res].time && currentTime < keys[res + 1].time)
			{
				lastPosition = res;
				return lastPosition;
			}

			res++;
			if (res == keys.size() - 1) res = 0;
		} while (res != lastPosition);

		return 0;
	}
}