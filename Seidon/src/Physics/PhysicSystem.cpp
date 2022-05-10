#include "PhysicSystem.h"

#include "../Core/Application.h"
#include "../Ecs/Entity.h"

using namespace physx;

namespace Seidon
{
	void PhysicSystem::Init()
	{
		api = Application::Get()->GetPhysicsApi();
		physics = api->GetPhysics();

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
		characterControllerCallbacks = new CharacterControllerCallbacks();

		/*
			Cube Colliders
		*/
		scene->CreateViewAndIterate<CubeColliderComponent>
		(
			[&](EntityId id, CubeColliderComponent&)
			{
				SetupCubeCollider(id);
			}
		);

		cubeColliderAddedCallbackId = scene->AddComponentAddedCallback<CubeColliderComponent>([&](EntityId id)
			{
				SetupCubeCollider(id);
			});

		cubeColliderRemovedCallbackId = scene->AddComponentRemovedCallback<CubeColliderComponent>([&](EntityId id)
			{
				DeleteCubeCollider(id);
			});

		/*
			Static Rigidbodies
		*/
		scene->CreateViewAndIterate<StaticRigidbodyComponent>
		(
				[&](EntityId id, StaticRigidbodyComponent&)
				{
					SetupStaticRigidbody(id);
				}
		);

		staticRigidbodyAddedCallbackId = scene->AddComponentAddedCallback<StaticRigidbodyComponent>([&](EntityId id)
			{
				SetupStaticRigidbody(id);
			});

		staticRigidbodyRemovedCallbackId = scene->AddComponentRemovedCallback<StaticRigidbodyComponent>([&](EntityId id)
			{
				DeleteStaticRigidbody(id);
			});

		/*
			Dynamic Rigidbodies
		*/
		scene->CreateViewAndIterate<DynamicRigidbodyComponent>
			(
				[&](EntityId id, DynamicRigidbodyComponent&)
				{
					SetupDynamicRigidbody(id);
				}
		);

		dynamicRigidbodyAddedCallbackId = scene->AddComponentAddedCallback<DynamicRigidbodyComponent>([&](EntityId id)
			{
				SetupDynamicRigidbody(id);
			});

		dynamicRigidbodyRemovedCallbackId = scene->AddComponentRemovedCallback<DynamicRigidbodyComponent>([&](EntityId id)
			{
				DeleteDynamicRigidbody(id);
			});

		/*
			Character controllers
		*/
		scene->CreateViewAndIterate<CharacterControllerComponent>
		(
				[&](EntityId id, CharacterControllerComponent&)
				{
					SetupCharacterController(id);
				}
		);

		characterControllerAddedCallbackId = scene->AddComponentAddedCallback<CharacterControllerComponent>([&](EntityId id)
			{
				SetupCharacterController(id);
			});

		characterControllerRemovedCallbackId = scene->AddComponentRemovedCallback<CharacterControllerComponent>([&](EntityId id)
			{
				DeleteCharacterController(id);
			});
	}
	 
	void PhysicSystem::Update(float deltaTime)
	{
		timeSinceLastStep += deltaTime;
		if (timeSinceLastStep < stepSize) return;

		timeSinceLastStep -= stepSize;

		scene->CreateGroupAndIterate<DynamicRigidbodyComponent>
		(
			GetTypeList<TransformComponent>,

			[&](EntityId e, DynamicRigidbodyComponent& rigidbody, TransformComponent& transform)
			{
				PxTransform t;
				t.p = PxVec3(transform.position.x, transform.position.y, transform.position.z);
				glm::quat q = glm::quat(transform.rotation);
				t.q = PxQuat(q.x, q.y, q.z, q.w);

				PxRigidDynamic& actor = *(PxRigidDynamic*)rigidbody.runtimeBody;
				if (rigidbody.kinematic)
				{
					actor.setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, true);
					actor.setKinematicTarget(t);
				}
				else
				{
					actor.setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, false);
					actor.setGlobalPose(t);
				}

				actor.setRigidDynamicLockFlag(PxRigidDynamicLockFlag::eLOCK_ANGULAR_X, rigidbody.lockXRotation);
				actor.setRigidDynamicLockFlag(PxRigidDynamicLockFlag::eLOCK_ANGULAR_Y, rigidbody.lockYRotation);
				actor.setRigidDynamicLockFlag(PxRigidDynamicLockFlag::eLOCK_ANGULAR_Z, rigidbody.lockZRotation);
			}
		);
		
		scene->CreateGroupAndIterate<CharacterControllerComponent>
		(
			GetTypeList<TransformComponent>,

			[&](EntityId e, CharacterControllerComponent& characterController, TransformComponent& transform)
			{
				characterController.collisions.clear();
			}
		);

		physxScene->simulate(stepSize);
		physxScene->fetchResults(true);

		uint32_t actorCount = 0;
		PxActor** activeActors = physxScene->getActiveActors(actorCount);

		for (int i = 0; i < actorCount; i++)
		{
			PxRigidActor* actor = (PxRigidActor*)activeActors[i];
			PxTransform transform = actor->getGlobalPose();

			EntityId id = (EntityId)(int)actor->userData;

			Entity e = scene->GetEntityByEntityId(id);
			
			TransformComponent& t = e.GetComponent<TransformComponent>();
			t.position = glm::vec3(transform.p.x, transform.p.y, transform.p.z);
			t.rotation = glm::eulerAngles(glm::quat(transform.q.w, transform.q.x, transform.q.y, transform.q.z));
		}
	}

	void PhysicSystem::Destroy()
	{
		scene->CreateViewAndIterate<StaticRigidbodyComponent>
		(
				[&](EntityId id, StaticRigidbodyComponent&)
				{
					DeleteStaticRigidbody(id);
				}
		);

		scene->CreateViewAndIterate<DynamicRigidbodyComponent>
		(
				[&](EntityId id, DynamicRigidbodyComponent&)
				{
					DeleteDynamicRigidbody(id);
				}
		);

		scene->CreateViewAndIterate<CubeColliderComponent>
		(
				[&](EntityId id, CubeColliderComponent&)
				{
					DeleteCubeCollider(id);
				}
		);

		scene->CreateViewAndIterate<CharacterControllerComponent>
		(
				[&](EntityId id, CharacterControllerComponent&)
				{
					DeleteCharacterController(id);
				}
		);

		physxScene->release();

		scene->RemoveComponentAddedCallback<CubeColliderComponent>(cubeColliderAddedCallbackId);
		scene->RemoveComponentAddedCallback<StaticRigidbodyComponent>(staticRigidbodyAddedCallbackId);
		scene->RemoveComponentAddedCallback<DynamicRigidbodyComponent>(dynamicRigidbodyAddedCallbackId);
		scene->RemoveComponentAddedCallback<CharacterControllerComponent>(characterControllerAddedCallbackId);

		scene->RemoveComponentRemovedCallback<CubeColliderComponent>(cubeColliderRemovedCallbackId);
		scene->RemoveComponentRemovedCallback<StaticRigidbodyComponent>(staticRigidbodyRemovedCallbackId);
		scene->RemoveComponentRemovedCallback<DynamicRigidbodyComponent>(dynamicRigidbodyRemovedCallbackId);
		scene->RemoveComponentRemovedCallback<CharacterControllerComponent>(characterControllerRemovedCallbackId);
	}


	/*
	void PhysicSystem::SetupDynamicRigidbody(entt::entity entityId, TransformComponent& transform, MeshColliderComponent& meshCollider, DynamicRigidbodyComponent& rigidbody)
	{
		PxPhysics* physics = api->GetPhysics();

		glm::vec3 position = transform.position;
		PxTransform t;
		t.p = PxVec3(position.x, position.y, position.z);

		glm::quat rot = glm::quat(transform.rotation);
		t.q = PxQuat(rot.x, rot.y, rot.z, rot.w);

		glm::vec3 size = transform.scale;

		int vertexCount = 0;
		int indexCount = 0;
		for (SubMesh* submesh : meshCollider.mesh->subMeshes)
		{
			vertexCount += submesh->vertices.size();
			indexCount += submesh->indices.size();
		}

		std::vector<Vertex> vertices;
		std::vector<uint32_t> indices;

		vertices.resize(vertexCount);
		indices.resize(indexCount);

		int vertexOffset = 0;
		int indexOffset = 0;
		for (SubMesh* submesh : meshCollider.mesh->subMeshes)
		{
			memcpy(&vertices[vertexOffset], &submesh->vertices[0], submesh->vertices.size() * sizeof(Vertex));
			memcpy(&indices[indexOffset], &submesh->indices[0], submesh->vertices.size() * sizeof(int));

			vertexOffset += submesh->vertices.size();
			indexOffset += submesh->indices.size();
		}
		
		PxTriangleMeshDesc meshDesc;
		meshDesc.points.count = vertices.size();
		meshDesc.points.stride = sizeof(Vertex);
		meshDesc.points.data = &vertices[0];

		meshDesc.triangles.count = indices.size();
		meshDesc.triangles.stride = 3 * sizeof(int);
		meshDesc.triangles.data = &indices[0];

		PxDefaultMemoryOutputStream writeBuffer;
		PxTriangleMeshCookingResult::Enum result;

		bool status = api->GetCooker()->cookTriangleMesh(meshDesc, writeBuffer, &result);
		if (!status)
		{
			std::cerr << "Error cooking mesh " << meshCollider.mesh->name << std::endl;
			return;
		}

		PxDefaultMemoryInputData readBuffer(writeBuffer.getData(), writeBuffer.getSize());
		PxTriangleMesh* triangleMesh = physics->createTriangleMesh(readBuffer);

		PxMeshScale scale({ size.x, size.y, size.z }, PxQuat(PxIdentity));
		PxTriangleMeshGeometry geometry(triangleMesh, scale);

		auto actor = physics->createRigidDynamic(t);
		PxRigidBodyExt::setMassAndUpdateInertia(*actor, rigidbody.mass);

		PxRigidActorExt::createExclusiveShape(*actor, geometry, *defaultMaterial);

		if (rigidbody.lockXRotation) actor->setRigidDynamicLockFlag(PxRigidDynamicLockFlag::eLOCK_ANGULAR_X, true);
		if (rigidbody.lockYRotation) actor->setRigidDynamicLockFlag(PxRigidDynamicLockFlag::eLOCK_ANGULAR_Y, true);
		if (rigidbody.lockZRotation) actor->setRigidDynamicLockFlag(PxRigidDynamicLockFlag::eLOCK_ANGULAR_Z, true);

		actor->userData = (void*)entityId;
		physxScene->addActor(*actor);
		rigidbody.runtimeBody = actor;

	}
	*/
	void PhysicSystem::SetupCharacterController(EntityId id)
	{
		Entity e = scene->GetEntityByEntityId(id);
		TransformComponent& transform = e.GetComponent<TransformComponent>();
		CharacterControllerComponent& controller = e.GetComponent<CharacterControllerComponent>();

		PxCapsuleControllerDesc desc;
		desc.setToDefault();
		desc.height = controller.colliderHeight;
		desc.radius = controller.colliderRadius;
		desc.material = defaultMaterial;
		desc.position = { transform.position.x, transform.position.y, transform.position.z };
		desc.contactOffset = controller.contactOffset;
		desc.slopeLimit = glm::cos(glm::radians(controller.maxSlopeAngle));
		desc.userData = (void*)id;
		desc.reportCallback = characterControllerCallbacks;

		PxController* c = characterControllerManager->createController(desc);
		c->getActor()->userData = (void*)id;

		controller.runtimeController = c;
	}

	void PhysicSystem::SetupCubeCollider(EntityId id)
	{
		Entity e = scene->GetEntityByEntityId(id);
		TransformComponent& transform = e.GetComponent<TransformComponent>();
		CubeColliderComponent& cubeCollider = e.GetComponent<CubeColliderComponent>();

		glm::vec3 size = cubeCollider.halfExtents * transform.scale;
		PxBoxGeometry geometry = PxBoxGeometry(size.x, size.y, size.z);

		PxShape* shape = physics->createShape(geometry, *defaultMaterial, true);

		PxTransform t;
		t.p = PxVec3(cubeCollider.offset.x, cubeCollider.offset.y, cubeCollider.offset.z);
		shape->setLocalPose(t);

		cubeCollider.runtimeShape = shape;

		if (e.HasComponent<StaticRigidbodyComponent>())
		{
			StaticRigidbodyComponent& r = e.GetComponent<StaticRigidbodyComponent>();

			if (r.runtimeBody) ((PxRigidStatic*)r.runtimeBody)->attachShape(*shape);
		}

		if (e.HasComponent<DynamicRigidbodyComponent>())
		{
			DynamicRigidbodyComponent& r = e.GetComponent<DynamicRigidbodyComponent>();

			if (r.runtimeBody) ((PxRigidDynamic*)r.runtimeBody)->attachShape(*shape);
		}
	}

	void PhysicSystem::SetupStaticRigidbody(EntityId id)
	{
		Entity e = scene->GetEntityByEntityId(id);
		TransformComponent& transform = e.GetComponent<TransformComponent>();
		StaticRigidbodyComponent& rigidbody = e.GetComponent<StaticRigidbodyComponent>();

		PxTransform t;
		t.p = PxVec3(transform.position.x, transform.position.y, transform.position.z);

		glm::quat rot = glm::quat(transform.rotation);
		t.q = PxQuat(rot.x, rot.y, rot.z, rot.w);

		PxRigidStatic* actor = physics->createRigidStatic(t);
		actor->userData = (void*)id;

		if (e.HasComponent<CubeColliderComponent>())
		{
			CubeColliderComponent& cubeCollider = e.GetComponent<CubeColliderComponent>();

			if (cubeCollider.runtimeShape) actor->attachShape(*((PxShape*)cubeCollider.runtimeShape));
		}

		physxScene->addActor(*actor);
		rigidbody.runtimeBody = actor;
	}

	void PhysicSystem::SetupDynamicRigidbody(EntityId id)
	{
		Entity e = scene->GetEntityByEntityId(id);
		TransformComponent& transform = e.GetComponent<TransformComponent>();
		DynamicRigidbodyComponent& rigidbody = e.GetComponent<DynamicRigidbodyComponent>();

		PxTransform t;
		t.p = PxVec3(transform.position.x, transform.position.y, transform.position.z);

		glm::quat rot = glm::quat(transform.rotation);
		t.q = PxQuat(rot.x, rot.y, rot.z, rot.w);

		PxRigidDynamic* actor = physics->createRigidDynamic(t);
		PxRigidBodyExt::setMassAndUpdateInertia(*actor, rigidbody.mass);
		actor->userData = (void*)id;

		actor->setRigidDynamicLockFlag(PxRigidDynamicLockFlag::eLOCK_ANGULAR_X, rigidbody.lockXRotation);
		actor->setRigidDynamicLockFlag(PxRigidDynamicLockFlag::eLOCK_ANGULAR_Y, rigidbody.lockYRotation);
		actor->setRigidDynamicLockFlag(PxRigidDynamicLockFlag::eLOCK_ANGULAR_Z, rigidbody.lockZRotation);

		if (e.HasComponent<CubeColliderComponent>())
		{
			CubeColliderComponent& cubeCollider = e.GetComponent<CubeColliderComponent>();
			
			if (cubeCollider.runtimeShape)actor->attachShape(*((PxShape*)cubeCollider.runtimeShape));
		}

		physxScene->addActor(*actor);
		rigidbody.runtimeBody = actor;
	}

	void PhysicSystem::DeleteCubeCollider(EntityId id)
	{
		Entity e = scene->GetEntityByEntityId(id);
		CubeColliderComponent& cubeCollider = e.GetComponent<CubeColliderComponent>();

		PxShape* shape = (PxShape*)cubeCollider.runtimeShape;

		if (e.HasComponent<StaticRigidbodyComponent>())
		{
			StaticRigidbodyComponent& r = e.GetComponent<StaticRigidbodyComponent>();
			if (r.runtimeBody) ((PxRigidStatic*)r.runtimeBody)->detachShape(*shape);
		}

		if (e.HasComponent<DynamicRigidbodyComponent>())
		{
			DynamicRigidbodyComponent& r = e.GetComponent<DynamicRigidbodyComponent>();
			if (r.runtimeBody) ((PxRigidDynamic*)r.runtimeBody)->detachShape(*shape);
		}

		shape->release();
		cubeCollider.runtimeShape = nullptr;
	}

	void PhysicSystem::DeleteStaticRigidbody(EntityId id)
	{
		Entity e = scene->GetEntityByEntityId(id);
		StaticRigidbodyComponent& rigidbody = e.GetComponent<StaticRigidbodyComponent>();

		PxRigidActor* actor = (PxRigidActor*)rigidbody.runtimeBody;
		actor->release();

		rigidbody.runtimeBody = nullptr;
	}

	void PhysicSystem::DeleteDynamicRigidbody(EntityId id)
	{
		Entity e = scene->GetEntityByEntityId(id);
		DynamicRigidbodyComponent& rigidbody = e.GetComponent<DynamicRigidbodyComponent>();

		PxRigidActor* actor = (PxRigidActor*)rigidbody.runtimeBody;
		actor->release();

		rigidbody.runtimeBody = nullptr;
	}

	void PhysicSystem::DeleteCharacterController(EntityId id)
	{
		Entity e = scene->GetEntityByEntityId(id);
		CharacterControllerComponent& controller = e.GetComponent<CharacterControllerComponent>();

		PxController* c = (PxController*)controller.runtimeController;
		c->release();

		controller.runtimeController = nullptr;
	}
}
