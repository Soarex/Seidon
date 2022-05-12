#include "PhysicsShape.h"

#include <Physx/PxPhysicsAPI.h>

namespace Seidon
{
	physx::PxShape* PhysicsShape::GetInternalShape()
	{
		return physxShape;
	}
}