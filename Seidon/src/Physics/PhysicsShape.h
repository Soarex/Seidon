#pragma once

namespace physx
{
	class PxRigidDynamic;
	class PxShape;
	class PxGeometry;
}

namespace Seidon
{
	class PhysicsShape
	{
	private:
		physx::PxShape* physxShape = nullptr;

		bool initialized = false;
	public:
		inline bool IsInitialized() { return initialized; }
		physx::PxShape* GetInternalShape();

		friend class PhysicSystem;
	};
}