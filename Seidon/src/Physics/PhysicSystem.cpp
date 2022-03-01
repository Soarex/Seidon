#include "PhysicSystem.h"

#include <Physx/PxPhysicsAPI.h>
#include <Physx/extensions/PxDefaultErrorCallback.h>
#include <Physx/extensions/PxDefaultAllocator.h>

using namespace physx;

static PxDefaultErrorCallback defaultErrorCallback;
static PxDefaultAllocator defaultAllocatorCallback;

namespace Seidon
{
	void PhysicSystem::Init()
	{
		
		PxFoundation* mFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, defaultAllocatorCallback, defaultErrorCallback);
		if (!mFoundation)
			std::cerr << "Physx initialization error" << std::endl;
			
	}

	void PhysicSystem::Update(float deltaTime)
	{

	}

	void PhysicSystem::Destroy()
	{

	}
}
