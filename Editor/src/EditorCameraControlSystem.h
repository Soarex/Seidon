#pragma once
#include <Seidon.h>

namespace Seidon
{
	class EditorCameraControlSystem : public System
	{
	public:
		float movementSpeed, mouseSensitivity, scrollWheelSensitivity, panSpeed;
	private:
		Entity camera;

		bool rotationEnabled = true, movementEnabled = true;

		float pitch = 0, yaw = 0;
	public:
		EditorCameraControlSystem(float movementSpeed = 1, float mouseSensitivity = 0.1f, float scrollWheelSensitivity = 100, float panSpeed = 0.8f);
		void Init();
		void Update(float deltaTime);

		inline void SetRotationEnabled(bool value) { rotationEnabled = value; };
		inline void SetMovementEnabled(bool value) { movementEnabled = value; };

		inline void ToggleMovement() { movementEnabled = !movementEnabled; }
		inline void ToggleRotation() { rotationEnabled = !rotationEnabled; }

	};
}