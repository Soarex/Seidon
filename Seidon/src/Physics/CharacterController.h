#pragma once
#include "PhysxSceneQueryResult.h"
#include <glm/glm.hpp>


namespace physx
{
	class PxController;
	class PxScene;
	class PxGeometry;
}

namespace Seidon
{
	class CharacterController
	{
	private:
		physx::PxController* physxController = nullptr;
		physx::PxScene* referenceScene = nullptr;

		bool initialized = false;

	public:
		CharacterController() = default;

		inline bool IsInitialized() { return initialized; }
		physx::PxController* GetInternalController();

		bool Raycast(const glm::vec3& origin, const glm::vec3& direction, float lenght, RaycastResult& result);
		bool Overlap(const physx::PxGeometry& shape, const glm::vec3& shapePosition, const glm::vec3& shapeRotation, OverlapResult& result);
		bool Sweep(const physx::PxGeometry& shape, const glm::vec3& shapePosition, const glm::vec3& shapeRotation, const glm::vec3& direction, float distance, SweepResult& result);

		friend class PhysicSystem;
	};
}