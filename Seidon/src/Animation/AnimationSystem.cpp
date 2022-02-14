#include "AnimationSystem.h"

#include "Ecs/Scene.h"
#include "Ecs/Components.h"

#include <glm/gtx/quaternion.hpp>
#include <iostream>

namespace Seidon
{
	void AnimationSystem::Init()
	{
		for (int i = 0; i < 100; i++)
			localBoneMatrices[i] = glm::identity<glm::mat4>();
	}

	void AnimationSystem::Update(float deltaTime)
	{
		entt::basic_group group = scene->GetRegistry().group<AnimationComponent>(entt::get<SkinnedRenderComponent>);

		for (entt::entity e : group)
		{
			AnimationComponent& a = group.get<AnimationComponent>(e);
			SkinnedRenderComponent& r = group.get<SkinnedRenderComponent>(e);
			a.runtimeBoneMatrices.clear();

			Animation* animation = a.animation;
			Armature* armature = r.armature;

			float duration = animation->duration * animation->ticksPerSecond;

			a.runtimeTime += animation->ticksPerSecond * deltaTime;
			float timePoint = fmod(a.runtimeTime, animation->duration);

			int channelIndex = 0;
			for (int i = 0; i < armature->bones.size(); i++)
			{
				BoneData& bone = armature->bones[i];

				glm::mat4 parentTransform = glm::identity<glm::mat4>();
				glm::mat4 animationTransform = glm::identity<glm::mat4>();

				if (i > 0)
					parentTransform = localBoneMatrices[bone.parentId];

				//while (channelIndex < animation->channels.size() - 1 && animation->channels[channelIndex].boneId != bone.id)
					//channelIndex++;
				channelIndex = i;

				if (animation->channels[channelIndex].boneId == bone.id)
				{
					//std::cout << "Bone: " << r.armature->bones[i].name << std::endl;
					AnimationChannel& channel = animation->channels[channelIndex];
					int keyIndex = GetPositionKeyIndex(channel.positionKeys, timePoint, lastPositionKeyIndices[i]);
					glm::mat4 translation = glm::translate(glm::identity<glm::mat4>(), channel.positionKeys[keyIndex].value);
					lastPositionKeyIndices[i] = keyIndex;
					//std::cout << "Translation " << keyIndex << ": " << channel.positionKeys[keyIndex].value.x << ", " << channel.positionKeys[keyIndex].value.y << ", " << channel.positionKeys[keyIndex].value.z << std::endl;

					keyIndex = GetRotationKeyIndex(channel.rotationKeys, timePoint, lastRotationKeyIndices[i]);
					glm::mat4 rotation = glm::mat4_cast(channel.rotationKeys[keyIndex].value);
					lastRotationKeyIndices[i] = keyIndex;
					//std::cout << "Rotation " << keyIndex << ": " << channel.rotationKeys[keyIndex].value.x << ", " << channel.rotationKeys[keyIndex].value.y << ", " << channel.rotationKeys[keyIndex].value.z << ", " << channel.rotationKeys[keyIndex].value.w <<  std::endl;
					
					keyIndex = GetScalingKeyIndex(channel.scalingKeys, timePoint, lastScalingKeyIndices[i]);
					glm::mat4 scale = glm::scale(glm::identity<glm::mat4>(), channel.scalingKeys[keyIndex].value);
					lastScalingKeyIndices[i] = keyIndex;
					//std::cout << "Scale " << keyIndex << ": " << channel.scalingKeys[keyIndex].value.x << ", " << channel.scalingKeys[keyIndex].value.y << ", " << channel.scalingKeys[keyIndex].value.z << std::endl;

					animationTransform = translation * rotation * scale;

					//std::cout << std::endl;
				}
				localBoneMatrices[i] = parentTransform * animationTransform;

				/*
				std::cout << "Bone: " << r.armature->bones[i].name << std::endl;
				for (int j = 0; j < 4; j++)
					std::cout << "[" << animationTransform[j].x << ", " << animationTransform[j].y << ", " << animationTransform[j].z << ", " << animationTransform[j].w << "]" << std::endl;
				std::cout << std::endl;
				*/

				a.runtimeBoneMatrices.push_back(/*animation->sceneTransform */ localBoneMatrices[i] * bone.inverseBindPoseMatrix /* glm::inverse(animation->sceneTransform)*/);

			}
			/*
			std::cout << "Frame: " << timePoint << std::endl;
			for (int i = 0; i < a.runtimeBoneMatrices.size(); i++)
			{
				std::cout << "Bone: " << r.armature->bones[i].name << std::endl;
				for (int j = 0; j < 4; j++)
					std::cout << "[" << a.runtimeBoneMatrices[i][j].x << ", " << a.runtimeBoneMatrices[i][j].y << ", " << a.runtimeBoneMatrices[i][j].z << ", " << a.runtimeBoneMatrices[i][j].w << "]" << std::endl;
				std::cout << std::endl;
			}
			std::cout << std::endl;
			*/
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