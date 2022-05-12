#pragma once
#include "../Ecs/EnttWrappers.h"
#include <glm/glm.hpp>

namespace Seidon
{
	struct RaycastResult
	{
		EntityId hitEntityId;
		float hitDistance;
		glm::vec3 hitPosition;
		glm::vec3 hitNormal;
	};

	struct OverlapResult
	{
		EntityId hitEntityId;
	};

	struct SweepResult
	{
		EntityId hitEntityId;
		float hitDistance;
		glm::vec3 hitPosition;
		glm::vec3 hitNormal;
	};

}