#pragma once

#include <glm/glm.hpp>


namespace physx
{
	class PxRigidStatic;
	class PxScene;
	class PxGeometry;
}

namespace Seidon
{
	class StaticActor
	{
	private:
		physx::PxRigidStatic* physxActor = nullptr;
		physx::PxScene* referenceScene = nullptr;

		bool initialized = false;
	public:
		StaticActor() = default;

		inline bool IsInitialized() { return initialized; }
		physx::PxRigidStatic* GetInternalActor();

		friend class PhysicSystem;
	};
}