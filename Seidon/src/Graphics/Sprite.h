#pragma once
#include "Texture.h"
#include <glm/glm.hpp>

namespace Seidon
{
	struct Sprite
	{
		UUID id;
		glm::vec3 tint;
		Texture* texture;
	};
}