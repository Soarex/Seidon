#include "FlyingCameraControlSystem.h"
#include "../Ecs/Scene.h"

namespace Seidon
{
	FlyingCameraControlSystem::FlyingCameraControlSystem(float speed, float mouseSensitivity)
		:speed(speed), mouseSensitivity(mouseSensitivity)
	{
	}

	void FlyingCameraControlSystem::Init()
	{
		entt::basic_group cameras = scene->GetRegistry().group<CameraComponent, TransformComponent>();

		if (!cameras.empty())
			camera = Entity(cameras.front(), scene);
	}
	 
	void FlyingCameraControlSystem::Update(float deltaTime)
	{
		if (rotationEnabled)
		{
			glm::vec2 mouseOffset = inputManager->GetMouseOffset();

			pitch += mouseOffset.y * mouseSensitivity;
			yaw -= mouseOffset.x * mouseSensitivity;

			if (pitch > 89.0f)
				pitch = 89.0f;
			if (pitch < -89.0f)
				pitch = -89.0f;
		}

		glm::vec3 input;
		if(movementEnabled)
		{
			input.x = -inputManager->GetKey(GET_KEYCODE(A)) + inputManager->GetKey(GET_KEYCODE(D));
			input.z = -inputManager->GetKey(GET_KEYCODE(S)) + inputManager->GetKey(GET_KEYCODE(W));
		}
		

		TransformComponent& cameraTransform = camera.GetComponent<TransformComponent>();
		if (movementEnabled || rotationEnabled)
		{
			cameraTransform.rotation.x = glm::radians(pitch);
			cameraTransform.rotation.y = glm::radians(yaw);

			glm::vec3 forward = cameraTransform.GetForwardDirection();
			forward = glm::normalize(forward);

			glm::vec3 right = cameraTransform.GetRightDirection();
			right = glm::normalize(right);

			glm::vec3 direction = forward * input.z + right * input.x;
			//direction = glm::normalize(direction);

			cameraTransform.position += direction * speed * deltaTime;
		}
	}

	void FlyingCameraControlSystem::Destroy()
	{

	}
}