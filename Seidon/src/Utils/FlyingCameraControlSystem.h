#pragma once

#include "../Core/System.h"
#include "../Core/Entity.h"
#include "../Core/InputManager.h"

namespace Seidon
{
	class FlyingCameraControlSystem : public System
	{
	private:
		Entity camera;

		bool rotationEnabled = true, movementEnabled = true;

		float pitch = 0, yaw = 0;
		float speed, mouseSensitivity;
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