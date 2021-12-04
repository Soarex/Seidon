#pragma once

#include "../Core/Application.h"
#include "../Core/InputManager.h"
#include "../Ecs/System.h"
#include "../Ecs/Entity.h"

namespace Seidon
{
	class FlyingCameraControlSystem : public System
	{
	public:
		float speed, mouseSensitivity;

	private:
		Entity camera;

		bool rotationEnabled = true, movementEnabled = true;

		float pitch = 0, yaw = 0;
	public:
		FlyingCameraControlSystem(float speed = 1, float mouseSensitivity = 0.1f);
		void Init();
		void Update(float deltaTime);
		void Destroy();

		inline void SetRotationEnabled(bool value) { rotationEnabled = value; };
		inline void SetMovementEnabled(bool value) { movementEnabled = value; };

		inline void ToggleMovement() { movementEnabled = !movementEnabled; }
		inline void ToggleRotation() { rotationEnabled = !rotationEnabled; }
	};
}