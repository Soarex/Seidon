#pragma once

#define PVD_HOST "127.0.0.1"

#include <Physx/PxPhysicsAPI.h>
#include <Physx/extensions/PxDefaultErrorCallback.h>
#include <Physx/extensions/PxDefaultAllocator.h>

namespace Seidon
{
	class PhysicsApi
	{
	private:
		physx::PxFoundation* foundation;
		physx::PxPvd* pvd;
		physx::PxPhysics* physics;
		physx::PxCooking* cooking;
		

		physx::PxDefaultErrorCallback defaultErrorCallback;
		physx::PxDefaultAllocator defaultAllocatorCallback;

	public:
		void Init();
		void Destroy();

		inline physx::PxPhysics* GetPhysics() { return physics; };
	};
}