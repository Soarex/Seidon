#include "InputManager.h"

namespace Seidon
{
	constexpr unsigned int InputManager::KEY_STATE_COUNT;

	glm::vec2 InputManager::mousePosition;
	glm::vec2 InputManager::mouseOffset;
	bool	InputManager::receivedThisFrame;

	KeyState InputManager::keyStates[KEY_STATE_COUNT];
	bool InputManager::pressedThisFrame[KEY_STATE_COUNT];

	void InputManager::Init()
	{
		for (int i = 0; i < KEY_STATE_COUNT; i++)
			keyStates[i] = KeyState::RELEASED;

		Window::AddKeyboardCallback([](int keycode, int action)
			{
				if (action == GLFW_PRESS)
				{
					keyStates[keycode] = KeyState::PRESSED;
					pressedThisFrame[keycode] = true;
				}

				if (action == GLFW_RELEASE)
					keyStates[keycode] = KeyState::RELEASED;
			});

		Window::AddMouseCallback([](float posX, float posY)
			{
				mouseOffset.x = posX - mousePosition.x;
				mouseOffset.y = mousePosition.y - posY;

				mousePosition.x = posX;
				mousePosition.y = posY;

				receivedThisFrame = true;
			});
	}

	bool InputManager::GetKey(int keycode)
	{
		return keyStates[keycode] == KeyState::PRESSED || keyStates[keycode] == KeyState::DOWN;
	}

	bool InputManager::GetKeyPressed(int keycode)
	{
		return keyStates[keycode] == KeyState::PRESSED;
	}

	bool InputManager::GetKeyDown(int keycode)
	{
		return keyStates[keycode] == KeyState::DOWN;
	}

	bool InputManager::GetKeyReleased(int keycode)
	{
		return keyStates[keycode] == KeyState::RELEASED;
	}

	const glm::vec2& InputManager::GetMousePosition()
	{
		return mousePosition;
	}

	const glm::vec2& InputManager::GetMouseOffset()
	{
		return mouseOffset;
	}

	void InputManager::Update()
	{
		for (int i = 0; i < KEY_STATE_COUNT; i++)
		{
			if (keyStates[i] == KeyState::PRESSED && pressedThisFrame[i] == false)
				keyStates[i] = KeyState::DOWN;

			pressedThisFrame[i] = false;
		}

		if (receivedThisFrame) receivedThisFrame = false;
		else mouseOffset.x = mouseOffset.y = 0;
	}
}