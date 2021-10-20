#pragma once
#include "Ecs/System.h"
#include "Ecs/Components.h"
#include "Ecs/Scene.h"

namespace Seidon
{
	class PhysicSystem : public System
	{
	private:

	public:
		PhysicSystem() = default;

		void Init() {}
		void Update(float deltaTime) {}
		void Destroy() {}

	private:
		void SetupRigidbodies() {}
	};

}