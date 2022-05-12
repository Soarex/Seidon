#include "DynamicActor.h"
#include <Physx/PxPhysicsAPI.h>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

namespace Seidon
{
	DynamicActor::DynamicActor()
	{

	}
	physx::PxRigidDynamic* DynamicActor::GetInternalActor()
	{ 
		return physxActor; 
	}

	void DynamicActor::AddForce(const glm::vec3& force, ForceMode mode)
	{
		physx::PxForceMode::Enum fm = mode == ForceMode::FORCE ? physx::PxForceMode::eFORCE : physx::PxForceMode::eIMPULSE;

		physxActor->addForce({ force.x, force.y, force.z }, fm);
	}

	bool DynamicActor::Raycast(const glm::vec3& origin, const glm::vec3& direction, float lenght, RaycastResult& result)
	{
		physx::PxRaycastBuffer hit;

		bool res = referenceScene->raycast
		(
			{ origin.x, origin.y, origin.z },
			{direction.x, direction.y, direction.z},
			lenght,
			hit
		);

		if (!res) return res;

		result.hitEntityId = (EntityId)(unsigned int)hit.block.actor->userData;
		result.hitDistance = hit.block.distance;
		result.hitPosition = { hit.block.position.x, hit.block.position.y, hit.block.position.z };
		result.hitNormal = { hit.block.normal.x,hit.block.normal.y, hit.block.normal.z };

		return res;
	}

	bool DynamicActor::Overlap(const physx::PxGeometry& shape, const glm::vec3& shapePosition, const glm::vec3& shapeRotation, OverlapResult& result)
	{
		physx::PxOverlapBuffer hit;

		physx::PxTransform transform;
		transform.p = { shapePosition.x, shapePosition.y, shapePosition.z };

		glm::quat q = glm::quat(shapeRotation);
		transform.q = physx::PxQuat(q.x, q.y, q.z, q.w);

		bool res = referenceScene->overlap
		(
			shape,
			transform,
			hit
		);

		result.hitEntityId = (EntityId)(unsigned int)hit.block.actor->userData;

		return res;
	}

	bool DynamicActor::Sweep(const physx::PxGeometry& shape, const glm::vec3& shapePosition, const glm::vec3& shapeRotation, const glm::vec3& direction, float distance, SweepResult& result)
	{
		physx::PxSweepBuffer hit;

		physx::PxTransform transform;
		transform.p = { shapePosition.x, shapePosition.y, shapePosition.z };

		glm::quat q = glm::quat(shapeRotation);
		transform.q = physx::PxQuat(q.x, q.y, q.z, q.w);

		bool res = referenceScene->sweep
		(
			shape,
			transform,
			{ direction.x, direction.y, direction.z },
			distance,
			hit
		);
		result.hitEntityId = (EntityId)(unsigned int)hit.block.actor->userData;
		result.hitDistance = hit.block.distance;
		result.hitPosition = { hit.block.position.x, hit.block.position.y, hit.block.position.z };
		result.hitNormal = { hit.block.normal.x,hit.block.normal.y, hit.block.normal.z };

		return res;
	}
}