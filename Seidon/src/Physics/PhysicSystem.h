#pragma once

#include "PhysicsApi.h"
#include "CharacterControllerCallbacks.h"

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

		physx::PxPhysics* physics;
		physx::PxCpuDispatcher* dispatcher;
		physx::PxScene* physxScene;
		physx::PxControllerManager* characterControllerManager;

		physx::PxMaterial* defaultMaterial;

		CharacterControllerCallbacks* characterControllerCallbacks;

		ComponentCallbackId cubeColliderAddedCallbackId;
		ComponentCallbackId meshColliderAddedCallbackId;
		ComponentCallbackId staticRigidbodyAddedCallbackId;
		ComponentCallbackId dynamicRigidbodyAddedCallbackId;
		ComponentCallbackId characterControllerAddedCallbackId;

		ComponentCallbackId cubeColliderRemovedCallbackId;
		ComponentCallbackId meshColliderRemovedCallbackId;
		ComponentCallbackId staticRigidbodyRemovedCallbackId;
		ComponentCallbackId dynamicRigidbodyRemovedCallbackId;
		ComponentCallbackId characterControllerRemovedCallbackId;

		float timeSinceLastStep = 0;
	public:
		PhysicSystem() = default;

		void Init();
		void Update(float deltaTime);
		void Destroy();

	private:
		void SetupCubeCollider(EntityId id);
		void SetupMeshCollider(EntityId id);
		void SetupStaticRigidbody(EntityId id);
		void SetupDynamicRigidbody(EntityId id);
		void SetupCharacterController(EntityId id);

		void DeleteCubeCollider(EntityId id);
		void DeleteMeshCollider(EntityId id);
		void DeleteStaticRigidbody(EntityId id);
		void DeleteDynamicRigidbody(EntityId id);
		void DeleteCharacterController(EntityId id);
	};

}