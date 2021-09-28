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
		entt::basic_group cameras = scene->GetRegistry().group<CameraComponent, TransformComponent>();

		if (!cameras.empty())
			camera = Entity(cameras.front(), &scene->GetRegistry());
		else
			camera = { entt::null, nullptr };
	}

	void EditorCameraControlSystem::Update(float deltaTime)
	{
		TransformComponent& cameraTransform = camera.GetComponent<TransformComponent>();

		glm::vec3 forward = cameraTransform.GetForwardDirection();
		forward = glm::normalize(forward);

		glm::vec3 right = cameraTransform.GetRightDirection();
		right = glm::normalize(right);

		glm::vec3 up = cameraTransform.GetUpDirection();
		up = glm::normalize(up);

		if (InputManager::GetMouseButtonDown(MouseButton::RIGHT) || InputManager::GetMouseButtonDown(MouseButton::MIDDLE))
			Window::EnableMouseCursor(false);
		else
			Window::EnableMouseCursor(true);

		if (InputManager::GetMouseButtonDown(MouseButton::RIGHT))
		{
			glm::vec2 mouseOffset = InputManager::GetMouseOffset();

			pitch += mouseOffset.y * mouseSensitivity;
			yaw -= mouseOffset.x * mouseSensitivity;

			if (pitch > 89.0f)
				pitch = 89.0f;
			if (pitch < -89.0f)
				pitch = -89.0f;

			cameraTransform.rotation.x = glm::radians(pitch);
			cameraTransform.rotation.y = glm::radians(yaw);
		}

		glm::vec3 input = { 0, 0, 0 };
		if (InputManager::GetMouseButtonDown(MouseButton::RIGHT))
		{
			input.x = -InputManager::GetKey(GET_KEYCODE(A)) + InputManager::GetKey(GET_KEYCODE(D));
			input.z = -InputManager::GetKey(GET_KEYCODE(S)) + InputManager::GetKey(GET_KEYCODE(W));

			glm::vec3 direction = forward * input.z + right * input.x;

			cameraTransform.position += direction * movementSpeed * deltaTime;
		}


		if (InputManager::GetMouseButtonDown(MouseButton::MIDDLE))
		{
			input.x = -InputManager::GetMouseOffset().x;
			input.y = -InputManager::GetMouseOffset().y;

			glm::vec3 direction = input.x * right + input.y * up;

			cameraTransform.position += direction * panSpeed * deltaTime;
		}

		input.x = input.y = 0;
		input.z = InputManager::GetMouseWheelOffset().y;

		glm::vec3 direction = forward * input.z;

		cameraTransform.position += direction * scrollWheelSensitivity * deltaTime;
	}
}