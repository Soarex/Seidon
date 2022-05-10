#pragma once
#include "../Ecs/EnttWrappers.h"

#include <glm/glm.hpp>

namespace Seidon
{
	struct CollisionData
	{
		EntityId hitEntityId;
		glm::vec3 contactPosition;
		glm::vec3 contactNormal;

		glm::vec3 hitDirection;
		float hitLenght;
	};
}