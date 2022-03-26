#pragma once

#include "PhysicsApi.h"

#include "Ecs/System.h"
#include "Ecs/Components.h"
#include "Ecs/Scene.h"

#include <Physx/PxPhysicsAPI.h>
#include <Physx/extensions/PxDefaultErrorCallback.h>
#include <Physx/extensions/PxDefaultAllocator.h>

namespace Seidon
{
	class PhysicSystem : public System
	{
	private:
		static constexpr float stepSize = 1.0f / 60.0f;

		PhysicsApi* api;

		physx::PxCpuDispatcher* dispatcher;
		physx::PxScene* physxScene;

		physx::PxMaterial* defaultMaterial;

		float timeSinceLastStep = 0;
	public:
		PhysicSystem() = default;

		void Init();
		void Update(float deltaTime);
		void Destroy();

	private:
		void SetupRigidbody(TransformComponent& transform, CubeColliderComponent& collider, RigidbodyComponent& rigidbody);
	};

}