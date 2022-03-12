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
		defaultMaterial = physics->createMaterial(0.5f, 0.5f, 0.6f);

		entt::basic_group group = scene->GetRegistry().group<CubeColliderComponent, RigidbodyComponent>(entt::get<TransformComponent>);

		for (entt::entity e : group)
		{
			auto [collider, rigidbody, transform] = group.get<CubeColliderComponent, RigidbodyComponent, TransformComponent>(e);

			if (!collider.runtimeShape)
				collider.runtimeShape = physics->createShape(PxBoxGeometry(collider.halfExtents.x, collider.halfExtents.y, collider.halfExtents.z), *defaultMaterial);

			PxTransform t;
			t.p = PxVec3(transform.position.x, transform.position.y, transform.position.z);

			glm::quat rot = glm::quat(transform.rotation);
			t.q = PxQuat(rot.x, rot.y, rot.z, rot.w);

			if (rigidbody.dynamic)
			{
				auto actor = PxCreateDynamic(*physics, t, *(PxShape*)collider.runtimeShape, rigidbody.mass);
				actor->userData = (void*)e;
				physxScene->addActor(*actor);
			}
			else
			{
				auto actor = PxCreateStatic(*physics, t, *(PxShape*)collider.runtimeShape);
				actor->userData = (void*)e;
				physxScene->addActor(*actor);
			}
		}

	}
	 
	void PhysicSystem::Update(float deltaTime)
	{
		timeSinceLastStep += deltaTime;
		if (timeSinceLastStep < stepSize) return;

		timeSinceLastStep -= stepSize;

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

		physxScene->simulate(stepSize);
		physxScene->fetchResults(true);

		nbActors = physxScene->getNbActors(PxActorTypeFlag::eRIGID_DYNAMIC | PxActorTypeFlag::eRIGID_STATIC);
		for (int i = 0; i < nbActors; i++)
		{
			std::vector<PxRigidActor*> actors(nbActors);
			physxScene->getActors(PxActorTypeFlag::eRIGID_DYNAMIC | PxActorTypeFlag::eRIGID_STATIC, reinterpret_cast<PxActor**>(&actors[0]), nbActors);
			PxTransform transform = actors[i]->getGlobalPose();

			Entity e((entt::entity)(int)actors[i]->userData, &scene->GetRegistry());

			TransformComponent& t = e.GetComponent<TransformComponent>();
			t.position = glm::vec3(transform.p.x, transform.p.y, transform.p.z);
			t.rotation = glm::eulerAngles(glm::quat(transform.q.w, transform.q.x, transform.q.y, transform.q.z));
		}
		
	}

	void PhysicSystem::Destroy()
	{
		physxScene->release();
	}
}
