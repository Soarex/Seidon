#include "PhysicsApi.h"
#include <iostream>

using namespace physx;

namespace Seidon
{
	void PhysicsApi::Init()
	{
		foundation = PxCreateFoundation(PX_PHYSICS_VERSION, defaultAllocatorCallback, defaultErrorCallback);

		if (!foundation)
		{
			std::cerr << "Physx foundation initialization error" << std::endl;
			return;
		}

		bool recordMemoryAllocations = true;

		pvd = PxCreatePvd(*foundation);
		PxPvdTransport* transport = PxDefaultPvdSocketTransportCreate(PVD_HOST, 5425, 10);
		pvd->connect(*transport, PxPvdInstrumentationFlag::eALL);

		PxTolerancesScale scale;
		physics = PxCreatePhysics(PX_PHYSICS_VERSION, *foundation, scale, recordMemoryAllocations, pvd);

		if (!physics)
		{
			std::cerr << "Physx physics initialization error" << std::endl;
			return;
		}

		cooking = PxCreateCooking(PX_PHYSICS_VERSION, *foundation, PxCookingParams(scale));

		if (!cooking)
		{
			std::cerr << "Physx cooking initialization error" << std::endl;
			return;
		}
		
		if (!PxInitExtensions(*physics, pvd))
		{
			std::cerr << "Physx extensions initialization error" << std::endl;
			return;
		}
		
		/*
		PxPvdSceneClient* pvdClient = physxScene->getScenePvdClient();
		if (pvdClient)
		{
			pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, true);
			pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONTACTS, true);
			pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES, true);
		}
		*/
	}

	void PhysicsApi::Destroy()
	{
		physics->release();
		cooking->release();
		pvd->release();
		foundation->release();
	}
}