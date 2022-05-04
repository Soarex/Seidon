#include "PhysicSystem.h"

#include "../Core/Application.h"
#include "../Ecs/Entity.h"

using namespace physx;

namespace Seidon
{
	void PhysicSystem::Init()
	{
		api = Application::Get()->GetPhysicsApi();
		PxPhysics* physics = api->GetPhysics();

		PxSceneDesc sceneDesc(physics->getTolerancesScale());
		sceneDesc.gravity = PxVec3(0.0f, -9.81f, 0.0f);
		dispatcher = PxDefaultCpuDispatcherCreate(2);
		sceneDesc.cpuDispatcher = dispatcher;
		sceneDesc.filterShader = PxDefaultSimulationFilterShader;

		physxScene = physics->createScene(sceneDesc);
		physxScene->setFlag(PxSceneFlag::eENABLE_ACTIVE_ACTORS, true);
		physxScene->setFlag(PxSceneFlag::eEXCLUDE_KINEMATICS_FROM_ACTIVE_ACTORS, true);

		defaultMaterial = physics->createMaterial(0.5f, 0.5f, 0.6f);

		characterControllerManager = PxCreateControllerManager(*physxScene);
	}
	 
	void PhysicSystem::Update(float deltaTime)
	{
		timeSinceLastStep += deltaTime;
		if (timeSinceLastStep < stepSize) return;

		timeSinceLastStep -= stepSize;

		entt::basic_group rigidbodies = scene->GetRegistry().group<CubeColliderComponent, RigidbodyComponent>(entt::get<TransformComponent>);
		entt::basic_group characterControllers = scene->GetRegistry().group<CharacterControllerComponent>(entt::get<TransformComponent>);

		for (entt::entity e : rigidbodies)
		{
			auto [collider, rigidbody, transform] = rigidbodies.get<CubeColliderComponent, RigidbodyComponent, TransformComponent>(e);

			if (rigidbody.runtimeBody == nullptr) SetupRigidbody(e, transform, collider, rigidbody);

			PxTransform t;
			t.p = PxVec3(transform.position.x, transform.position.y, transform.position.z);
			glm::quat q = glm::quat(transform.rotation);
			t.q = PxQuat(q.x, q.y, q.z, q.w);

			((PxRigidActor*)rigidbody.runtimeBody)->setGlobalPose(t);
		}
		
		for (entt::entity e : characterControllers)
		{
			auto [characterController, transform] = characterControllers.get<CharacterControllerComponent, TransformComponent>(e);

			if (characterController.runtimeController == nullptr) SetupCharacterController(e, transform, characterController);

			PxExtendedVec3 p = PxExtendedVec3(transform.position.x, transform.position.y, transform.position.z);
			PxVec3 newPosition = p - ((PxController*)characterController.runtimeController)->getPosition();

			PxControllerCollisionFlags collisionFlags = ((PxController*)characterController.runtimeController)->move(newPosition, 0.01, deltaTime, PxControllerFilters());
			characterController.isGrounded = collisionFlags.isSet(PxControllerCollisionFlag::eCOLLISION_DOWN);
		}
		/*
		PxU32 nbActors = physxScene->getNbActors(PxActorTypeFlag::eRIGID_DYNAMIC | PxActorTypeFlag::eRIGID_STATIC);
		for (int i = 0; i < nbActors; i++)
		{
			std::vector<PxRigidActor*> actors(nbActors);
			physxScene->getActors(PxActorTypeFlag::eRIGID_DYNAMIC | PxActorTypeFlag::eRIGID_STATIC, reinterpret_cast<PxActor**>(&actors[0]), nbActors);
			PxTransform transform;

			Entity e((entt::entity)(int)actors[i]->userData, &scene->GetRegistry());
			TransformComponent& t = e.GetComponent<TransformComponent>();

			transform.p = PxVec3(t.position.x, t.position.y, t.position.z);
			glm::quat q = glm::quat(t.rotation);
			transform.q = PxQuat(q.x, q.y, q.z, q.w);

			actors[i]->setGlobalPose(transform);
		}
		*/
		physxScene->simulate(stepSize);
		physxScene->fetchResults(true);

		uint32_t actorCount;
		PxRigidActor** actors = (PxRigidActor**)physxScene->getActiveActors(actorCount);
		for (int i = 0; i < actorCount; i++)
		{
			PxTransform transform = actors[i]->getGlobalPose();

			Entity e((entt::entity)(uint32_t)actors[i]->userData, &scene->GetRegistry());

			TransformComponent& t = e.GetComponent<TransformComponent>();
			t.position = glm::vec3(transform.p.x, transform.p.y, transform.p.z);
			t.rotation = glm::eulerAngles(glm::quat(transform.q.w, transform.q.x, transform.q.y, transform.q.z));
		}

		for (entt::entity e : characterControllers)
		{
			auto [characterController, transform] = characterControllers.get<CharacterControllerComponent, TransformComponent>(e);

			PxExtendedVec3 p = ((PxController*)characterController.runtimeController)->getPosition();

			transform.position = { p.x, p.y, p.z };
		}

		/*
		for (entt::entity e : group)
		{
			auto [collider, rigidbody, transform] = group.get<CubeColliderComponent, RigidbodyComponent, TransformComponent>(e);

			PxTransform t = ((PxRigidActor*)rigidbody.runtimeBody)->getGlobalPose();

			transform.position = glm::vec3(t.p.x, t.p.y, t.p.z);
			transform.rotation = glm::eulerAngles(glm::quat(t.q.w, t.q.x, t.q.y, t.q.z));
		}
		*/
	}

	void PhysicSystem::Destroy()
	{
		physxScene->release();
	}

	void PhysicSystem::SetupRigidbody(entt::entity entityId, TransformComponent& transform, CubeColliderComponent& collider, RigidbodyComponent& rigidbody)
	{
		PxPhysics* physics = api->GetPhysics();

		glm::vec3 position = transform.position + collider.offset;
		PxTransform t;
		t.p = PxVec3(position.x, position.y, position.z);

		glm::quat rot = glm::quat(transform.rotation);
		t.q = PxQuat(rot.x, rot.y, rot.z, rot.w);

		glm::vec3 size = collider.halfExtents * transform.scale;
		PxBoxGeometry shape = PxBoxGeometry(size.x, size.y, size.z);
		
		if (rigidbody.dynamic)
		{
			auto actor = physics->createRigidDynamic(t);
			PxRigidBodyExt::setMassAndUpdateInertia(*actor, rigidbody.mass);

			PxRigidActorExt::createExclusiveShape(*actor, shape, *defaultMaterial);

			if (rigidbody.lockXRotation) actor->setRigidDynamicLockFlag(PxRigidDynamicLockFlag::eLOCK_ANGULAR_X, true);
			if (rigidbody.lockYRotation) actor->setRigidDynamicLockFlag(PxRigidDynamicLockFlag::eLOCK_ANGULAR_Y, true);
			if (rigidbody.lockZRotation) actor->setRigidDynamicLockFlag(PxRigidDynamicLockFlag::eLOCK_ANGULAR_Z, true);

			actor->userData = (void*)entityId;
			physxScene->addActor(*actor);
			rigidbody.runtimeBody = actor;
		}
		else
		{
			auto actor = physics->createRigidStatic(t);
			PxRigidActorExt::createExclusiveShape(*actor, shape, *defaultMaterial);
			
			actor->userData = (void*)entityId;
			physxScene->addActor(*actor);
			rigidbody.runtimeBody = actor;
		}
	}

	void PhysicSystem::SetupCharacterController(entt::entity entityId, TransformComponent& transform, CharacterControllerComponent& controller)
	{
		PxCapsuleControllerDesc desc;
		desc.setToDefault();
		desc.height = controller.colliderHeight;
		desc.radius = controller.colliderRadius;
		desc.material = defaultMaterial;
		desc.position = { transform.position.x, transform.position.y, transform.position.z };
		desc.userData = (void*)entityId;

		PxController* c = characterControllerManager->createController(desc);
		controller.runtimeController = c;
	}
}
