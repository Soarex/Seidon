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
			Mesh Colliders
		*/
		scene->CreateViewAndIterate<MeshColliderComponent>
		(
			[&](EntityId id, MeshColliderComponent&)
			{
				SetupMeshCollider(id);
			}
		);

		meshColliderAddedCallbackId = scene->AddComponentAddedCallback<MeshColliderComponent>([&](EntityId id)
			{
				SetupMeshCollider(id);
			});

		meshColliderRemovedCallbackId = scene->AddComponentRemovedCallback<MeshColliderComponent>([&](EntityId id)
			{
				DeleteMeshCollider(id);
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

			[&](EntityId e, DynamicRigidbodyComponent& rigidbody, TransformComponent& localTransform)
			{
				TransformComponent transform;
				transform.SetFromMatrix(scene->GetEntityByEntityId(e).GetGlobalTransformMatrix());

				PxTransform t;
				t.p = PxVec3(transform.position.x, transform.position.y, transform.position.z);
				glm::quat q = glm::quat(transform.rotation);
				t.q = PxQuat(q.x, q.y, q.z, q.w);

				PxRigidDynamic& actor = *rigidbody.actor.physxActor;
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
			
			TransformComponent& localTransform = e.GetComponent<TransformComponent>();

			if (e.HasParent())
			{
				TransformComponent worldTransform;
				worldTransform.SetFromMatrix(e.GetGlobalTransformMatrix());
				worldTransform.position = glm::vec3(transform.p.x, transform.p.y, transform.p.z);
				worldTransform.rotation = glm::eulerAngles(glm::quat(transform.q.w, transform.q.x, transform.q.y, transform.q.z));

				localTransform.SetFromMatrix(glm::inverse(e.GetParent().GetGlobalTransformMatrix()) * worldTransform.GetTransformMatrix());
			}
			else
			{
				localTransform.position = glm::vec3(transform.p.x, transform.p.y, transform.p.z);
				localTransform.rotation = glm::eulerAngles(glm::quat(transform.q.w, transform.q.x, transform.q.y, transform.q.z));
			}
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
		scene->RemoveComponentAddedCallback<MeshColliderComponent>(meshColliderAddedCallbackId);
		scene->RemoveComponentAddedCallback<StaticRigidbodyComponent>(staticRigidbodyAddedCallbackId);
		scene->RemoveComponentAddedCallback<DynamicRigidbodyComponent>(dynamicRigidbodyAddedCallbackId);
		scene->RemoveComponentAddedCallback<CharacterControllerComponent>(characterControllerAddedCallbackId);

		scene->RemoveComponentRemovedCallback<CubeColliderComponent>(cubeColliderRemovedCallbackId);
		scene->RemoveComponentRemovedCallback<MeshColliderComponent>(meshColliderRemovedCallbackId);
		scene->RemoveComponentRemovedCallback<StaticRigidbodyComponent>(staticRigidbodyRemovedCallbackId);
		scene->RemoveComponentRemovedCallback<DynamicRigidbodyComponent>(dynamicRigidbodyRemovedCallbackId);
		scene->RemoveComponentRemovedCallback<CharacterControllerComponent>(characterControllerRemovedCallbackId);
	}


	
	void PhysicSystem::SetupMeshCollider(EntityId id)
	{
		Entity e = scene->GetEntityByEntityId(id);
		MeshColliderComponent& collider = e.GetComponent<MeshColliderComponent>();
		TransformComponent transform;
		transform.SetFromMatrix(e.GetGlobalTransformMatrix());

		glm::vec3 position = transform.position;
		PxTransform t;
		t.p = PxVec3(position.x, position.y, position.z);

		glm::quat rot = glm::quat(transform.rotation);
		t.q = PxQuat(rot.x, rot.y, rot.z, rot.w);

		glm::vec3 size = transform.scale;

		int vertexCount = 0;
		int indexCount = 0;
		for (SubMesh* submesh : collider.mesh->subMeshes)
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
		for (SubMesh* submesh : collider.mesh->subMeshes)
		{
			memcpy(&vertices[vertexOffset], &submesh->vertices[0], submesh->vertices.size() * sizeof(Vertex));

			for (int i = 0; i < submesh->indices.size(); i++)
				indices[indexOffset + i] = submesh->indices[i] + vertexOffset;

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

		std::cout << meshDesc.isValid() << std::endl;

		PxDefaultMemoryOutputStream writeBuffer;
		PxTriangleMeshCookingResult::Enum result;

		bool status = api->GetCooker()->cookTriangleMesh(meshDesc, writeBuffer, &result);
		if (!status)
		{
			std::cerr << "Error cooking mesh " << collider.mesh->name << std::endl;
			return;
		}

		PxDefaultMemoryInputData readBuffer(writeBuffer.getData(), writeBuffer.getSize());
		PxTriangleMesh* triangleMesh = physics->createTriangleMesh(readBuffer);

		PxMeshScale scale({ size.x, size.y, size.z }, PxQuat(PxIdentity));
		PxTriangleMeshGeometry geometry(triangleMesh, scale);

		PxShape* shape = physics->createShape(geometry, *defaultMaterial, true);

		collider.shape.physxShape = shape;
		collider.shape.initialized = true;


		if (e.HasComponent<StaticRigidbodyComponent>())
		{
			StaticRigidbodyComponent& r = e.GetComponent<StaticRigidbodyComponent>();

			if (r.actor.IsInitialized() && r.actor.referenceScene == physxScene) r.actor.GetInternalActor()->attachShape(*shape);
		}

		if (e.HasComponent<DynamicRigidbodyComponent>())
		{
			DynamicRigidbodyComponent& r = e.GetComponent<DynamicRigidbodyComponent>();

			if (r.actor.IsInitialized() && r.actor.referenceScene == physxScene) r.actor.GetInternalActor()->attachShape(*shape);
		}
	}

	void PhysicSystem::SetupCharacterController(EntityId id)
	{
		Entity e = scene->GetEntityByEntityId(id);
		CharacterControllerComponent& controller = e.GetComponent<CharacterControllerComponent>();

		TransformComponent transform;
		transform.SetFromMatrix(e.GetGlobalTransformMatrix());

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

		controller.runtimeController.physxController = c;
		controller.runtimeController.referenceScene = physxScene;
		controller.runtimeController.initialized = true;
	}

	void PhysicSystem::SetupCubeCollider(EntityId id)
	{
		Entity e = scene->GetEntityByEntityId(id);
		CubeColliderComponent& cubeCollider = e.GetComponent<CubeColliderComponent>();

		TransformComponent transform;
		transform.SetFromMatrix(e.GetGlobalTransformMatrix());

		glm::vec3 size = cubeCollider.halfExtents * transform.scale;
		PxBoxGeometry geometry = PxBoxGeometry(size.x, size.y, size.z);

		PxShape* shape = physics->createShape(geometry, *defaultMaterial, true);

		PxTransform t;
		t.p = PxVec3(cubeCollider.offset.x, cubeCollider.offset.y, cubeCollider.offset.z);
		t.q = PxQuat(PxIDENTITY());
		shape->setLocalPose(t);

		cubeCollider.shape.physxShape = shape;
		cubeCollider.shape.initialized = true;

		if (e.HasComponent<StaticRigidbodyComponent>())
		{
			StaticRigidbodyComponent& r = e.GetComponent<StaticRigidbodyComponent>();

			if (r.actor.IsInitialized() && r.actor.referenceScene == physxScene) r.actor.GetInternalActor()->attachShape(*shape);
		}

		if (e.HasComponent<DynamicRigidbodyComponent>())
		{
			DynamicRigidbodyComponent& r = e.GetComponent<DynamicRigidbodyComponent>();

			if (r.actor.IsInitialized() && r.actor.referenceScene == physxScene) r.actor.GetInternalActor()->attachShape(*shape);
		}
	}

	void PhysicSystem::SetupStaticRigidbody(EntityId id)
	{
		Entity e = scene->GetEntityByEntityId(id);
		StaticRigidbodyComponent& rigidbody = e.GetComponent<StaticRigidbodyComponent>();

		TransformComponent transform;
		transform.SetFromMatrix(e.GetGlobalTransformMatrix());

		PxTransform t;
		t.p = PxVec3(transform.position.x, transform.position.y, transform.position.z);

		glm::quat rot = glm::quat(transform.rotation);
		t.q = PxQuat(rot.x, rot.y, rot.z, rot.w);

		PxRigidStatic* actor = physics->createRigidStatic(t);
		actor->userData = (void*)id;

		if (e.HasComponent<CubeColliderComponent>())
		{
			CubeColliderComponent& collider = e.GetComponent<CubeColliderComponent>();

			if (collider.shape.IsInitialized()) actor->attachShape(*(collider.shape.GetInternalShape()));
		}

		if (e.HasComponent<MeshColliderComponent>())
		{
			MeshColliderComponent& collider = e.GetComponent<MeshColliderComponent>();

			if (collider.shape.IsInitialized()) actor->attachShape(*(collider.shape.GetInternalShape()));
		}

		physxScene->addActor(*actor);
		rigidbody.actor.physxActor = actor;
		rigidbody.actor.referenceScene = physxScene;
		rigidbody.actor.initialized = true;
	}

	void PhysicSystem::SetupDynamicRigidbody(EntityId id)
	{
		Entity e = scene->GetEntityByEntityId(id);
		DynamicRigidbodyComponent& rigidbody = e.GetComponent<DynamicRigidbodyComponent>();

		TransformComponent transform;
		transform.SetFromMatrix(e.GetGlobalTransformMatrix());

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
			CubeColliderComponent& collider = e.GetComponent<CubeColliderComponent>();

			if (collider.shape.IsInitialized())actor->attachShape(*collider.shape.GetInternalShape());
		}

		if (e.HasComponent<MeshColliderComponent>())
		{
			MeshColliderComponent& collider = e.GetComponent<MeshColliderComponent>();

			if (collider.shape.IsInitialized()) actor->attachShape(*(collider.shape.GetInternalShape()));
		}

		physxScene->addActor(*actor);
		rigidbody.actor.physxActor = actor;
		rigidbody.actor.referenceScene = physxScene;
		rigidbody.actor.initialized = true;
	}

	void PhysicSystem::DeleteCubeCollider(EntityId id)
	{
		Entity e = scene->GetEntityByEntityId(id);
		CubeColliderComponent& cubeCollider = e.GetComponent<CubeColliderComponent>();

		PxShape* shape = cubeCollider.shape.GetInternalShape();

		if (e.HasComponent<StaticRigidbodyComponent>())
		{
			StaticRigidbodyComponent& r = e.GetComponent<StaticRigidbodyComponent>();
			if (r.actor.IsInitialized()) r.actor.GetInternalActor()->detachShape(*shape);
		}

		if (e.HasComponent<DynamicRigidbodyComponent>())
		{
			DynamicRigidbodyComponent& r = e.GetComponent<DynamicRigidbodyComponent>();
			if (r.actor.IsInitialized()) r.actor.GetInternalActor()->detachShape(*shape);
		}

		shape->release();
		cubeCollider.shape.initialized = false;
		cubeCollider.shape.physxShape = nullptr;
	}

	void PhysicSystem::DeleteMeshCollider(EntityId id)
	{
		Entity e = scene->GetEntityByEntityId(id);
		MeshColliderComponent& collider = e.GetComponent<MeshColliderComponent>();

		PxShape* shape = collider.shape.GetInternalShape();

		if (e.HasComponent<StaticRigidbodyComponent>())
		{
			StaticRigidbodyComponent& r = e.GetComponent<StaticRigidbodyComponent>();
			if (r.actor.IsInitialized()) r.actor.GetInternalActor()->detachShape(*shape);
		}

		if (e.HasComponent<DynamicRigidbodyComponent>())
		{
			DynamicRigidbodyComponent& r = e.GetComponent<DynamicRigidbodyComponent>();
			if (r.actor.IsInitialized()) r.actor.GetInternalActor()->detachShape(*shape);
		}

		shape->release();
		collider.shape.initialized = false;
		collider.shape.physxShape = nullptr;
	}

	void PhysicSystem::DeleteStaticRigidbody(EntityId id)
	{
		Entity e = scene->GetEntityByEntityId(id);
		StaticRigidbodyComponent& rigidbody = e.GetComponent<StaticRigidbodyComponent>();

		PxRigidActor* actor = rigidbody.actor.GetInternalActor();
		actor->release();

		rigidbody.actor.initialized = false;
	}

	void PhysicSystem::DeleteDynamicRigidbody(EntityId id)
	{
		Entity e = scene->GetEntityByEntityId(id);
		DynamicRigidbodyComponent& rigidbody = e.GetComponent<DynamicRigidbodyComponent>();

		PxRigidActor* actor = rigidbody.actor.GetInternalActor();
		actor->release();

		rigidbody.actor.initialized = false;
	}

	void PhysicSystem::DeleteCharacterController(EntityId id)
	{
		Entity e = scene->GetEntityByEntityId(id);
		CharacterControllerComponent& controller = e.GetComponent<CharacterControllerComponent>();

		PxController* c = controller.runtimeController.GetInternalController();
		c->release();

		controller.runtimeController.physxController = nullptr;
		controller.runtimeController.referenceScene = nullptr;
		controller.runtimeController.initialized = false;
	}
}
