#pragma once
#include "PhysxSceneQueryResult.h"
#include <glm/glm.hpp>


namespace physx
{
	class PxRigidDynamic;
	class PxScene;
	class PxGeometry;
}

namespace Seidon
{
	enum class ForceMode
	{
		FORCE = 0,
		IMPULSE
	};

	class DynamicActor
	{
	private:
		physx::PxRigidDynamic* physxActor = nullptr;
		physx::PxScene* referenceScene = nullptr;

		bool initialized = false;
	public:
		DynamicActor();

		inline bool IsInitialized() { return initialized; }
		physx::PxRigidDynamic* GetInternalActor();

		void AddForce(const glm::vec3& force, ForceMode mode = ForceMode::FORCE);

		bool Raycast(const glm::vec3& origin, const glm::vec3& direction, float lenght, RaycastResult& result);
		bool Overlap(const physx::PxGeometry& shape, const glm::vec3& shapePosition, const glm::vec3& shapeRotation, OverlapResult& result);
		bool Sweep(const physx::PxGeometry& shape, const glm::vec3& shapePosition, const glm::vec3& shapeRotation, const glm::vec3& direction, float distance, SweepResult& result);
	
		friend class PhysicSystem;
	};
}