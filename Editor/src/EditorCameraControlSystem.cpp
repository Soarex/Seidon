#pragma once
#include "EditorCameraControlSystem.h"

namespace Seidon
{
	EditorCameraControlSystem::EditorCameraControlSystem(float movementSpeed, float mouseSensitivity, float scrollWheelSensitivity, float panSpeed)
		:movementSpeed(movementSpeed), mouseSensitivity(mouseSensitivity), scrollWheelSensitivity(scrollWheelSensitivity), panSpeed(panSpeed)
	{
	}

	void EditorCameraControlSystem::Init()
	{
	}

	void EditorCameraControlSystem::Update(float deltaTime)
	{
		entt::basic_group cameras = scene->GetRegistry().group<CameraComponent, TransformComponent>();

		for (entt::entity e : cameras)
		{
			Entity camera = { e, scene };
			TransformComponent& cameraTransform = camera.GetComponent<TransformComponent>();

			glm::vec3 forward = cameraTransform.GetForwardDirection();
			forward = glm::normalize(forward);

			glm::vec3 right = cameraTransform.GetRightDirection();
			right = glm::normalize(right);

			glm::vec3 up = cameraTransform.GetUpDirection();
			up = glm::normalize(up);

			if (inputManager->GetMouseButtonDown(MouseButton::RIGHT) || inputManager->GetMouseButtonDown(MouseButton::MIDDLE))
				window->EnableMouseCursor(false);
			else
				window->EnableMouseCursor(true);

			if (inputManager->GetMouseButtonDown(MouseButton::RIGHT))
			{
				glm::vec2 mouseOffset = inputManager->GetMouseOffset();

				pitch -= mouseOffset.y * mouseSensitivity;
				yaw -= mouseOffset.x * mouseSensitivity;

				if (pitch > 89.0f)
					pitch = 89.0f;
				if (pitch < -89.0f)
					pitch = -89.0f;

				cameraTransform.rotation.x = glm::radians(pitch);
				cameraTransform.rotation.y = glm::radians(yaw);
			}

			glm::vec3 input = { 0, 0, 0 };
			if (inputManager->GetMouseButtonDown(MouseButton::RIGHT))
			{
				input.x = -inputManager->GetKey(GET_KEYCODE(A)) + inputManager->GetKey(GET_KEYCODE(D));
				input.z = -inputManager->GetKey(GET_KEYCODE(S)) + inputManager->GetKey(GET_KEYCODE(W));

				glm::vec3 direction = forward * input.z + right * input.x;

				cameraTransform.position += direction * movementSpeed * deltaTime;
			}


			if (inputManager->GetMouseButtonDown(MouseButton::MIDDLE))
			{
				input.x = -inputManager->GetMouseOffset().x;
				input.y = -inputManager->GetMouseOffset().y;

				glm::vec3 direction = input.x * right + input.y * up;

				cameraTransform.position += direction * panSpeed * deltaTime;
			}

			input.x = input.y = 0;
			input.z = inputManager->GetMouseWheelOffset().y;

			glm::vec3 direction = forward * input.z;

			cameraTransform.position += direction * scrollWheelSensitivity * deltaTime;
		}
	}
}