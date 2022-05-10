#include "CharacterControllerCallbacks.h"
#include "CollisionData.h"

#include "../Core/Application.h"

namespace Seidon
{
	void CharacterControllerCallbacks::onShapeHit(const physx::PxControllerShapeHit& hit)
	{
		CollisionData data;
		data.hitEntityId = (EntityId)(int)hit.actor->userData;
		data.contactPosition = { hit.worldPos.x, hit.worldPos.y, hit.worldPos.z };
		data.contactNormal = { hit.worldNormal.x, hit.worldNormal.y, hit.worldNormal.z };
		data.hitDirection = { hit.dir.x, hit.dir.y, hit.dir.z };
		data.hitLenght = hit.length;

		Entity controllerEntity((EntityId)(int)hit.controller->getUserData(), Application::Get()->GetSceneManager()->GetActiveScene());
		controllerEntity.GetComponent<CharacterControllerComponent>().collisions.push_back(data);
	}

	void CharacterControllerCallbacks::onControllerHit(const physx::PxControllersHit& hit)
	{

	}

	void CharacterControllerCallbacks::onObstacleHit(const physx::PxControllerObstacleHit& hit)
	{

	}
}