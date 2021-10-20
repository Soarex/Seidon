#include "Entity.h"

namespace Seidon
{
	Entity::Entity(entt::entity id, entt::registry* registry)
		:ID(id), registry(registry) {}

}