#pragma once
#include <Physx/PxPhysicsAPI.h>

namespace Seidon
{
	class CharacterControllerCallbacks : public physx::PxUserControllerHitReport
	{
	public:
		void onShapeHit(const physx::PxControllerShapeHit& hit);
		void onControllerHit(const physx::PxControllersHit& hit);
		void onObstacleHit(const physx::PxControllerObstacleHit& hit);
	};
}