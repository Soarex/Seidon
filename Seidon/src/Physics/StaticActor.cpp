#include "StaticActor.h"
#include <Physx/PxPhysicsAPI.h>

namespace Seidon
{
	physx::PxRigidStatic* StaticActor::GetInternalActor()
	{
		return physxActor;
	}
}