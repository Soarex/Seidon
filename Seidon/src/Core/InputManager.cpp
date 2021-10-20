#include "InputManager.h"

namespace Seidon
{
	void InputManager::Init(Window* window)
	{
		for (int i = 0; i < KEY_STATE_COUNT; i++)
			keyStates[i] = KeyState::RELEASED;

		for (int i = 0; i < MOUSE_BUTTON_COUNT; i++)
			mouseButtonStates[i] = KeyState::RELEASED;

		window->AddKeyboardCallback([&](int keycode, int action)
			{
				if (action == GLFW_PRESS)
				{
					keyStates[keycode] = KeyState::PRESSED;
					keysPressedThisFrame[keycode] = true;
				}

				if (action == GLFW_RELEASE)
					keyStates[keycode] = KeyState::RELEASED;
			});

		window->AddCursorCallback([&](float posX, float posY)
			{
				mouseOffset.x = posX - mousePosition.x;
				mouseOffset.y = mousePosition.y - posY;

				mousePosition.x = posX;
				mousePosition.y = posY;

				cursorReceivedThisFrame = true;
			});

		window->AddMouseWheelCallback([&](float posX, float posY)
			{
				mouseWheelOffset.x = posX;
				mouseWheelOffset.y = posY;

				mouseWheelPosition.x += posX;
				mouseWheelPosition.y += posY;

				wheelReceivedThisFrame = true;
			});

		window->AddMouseButtonCallback([&](int button, int action)
			{
				if (button > 2) return;

				if (action == GLFW_PRESS)
				{
					mouseButtonStates[button] = KeyState::PRESSED;
					mouseButtonsPressedThisFrame[button] = true;
				}

				if (action == GLFW_RELEASE)
					mouseButtonStates[button] = KeyState::RELEASED;
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

	bool InputManager::GetMouseButton(MouseButton button)
	{
		return mouseButtonStates[(int)button] == KeyState::PRESSED || mouseButtonStates[(int)button] == KeyState::DOWN;
	}

	bool InputManager::GetMouseButtonPressed(MouseButton button)
	{
		return mouseButtonStates[(int)button] == KeyState::PRESSED;
	}

	bool InputManager::GetMouseButtonDown(MouseButton button)
	{
		return mouseButtonStates[(int)button] == KeyState::DOWN;
	}

	bool InputManager::GetMouseButtonReleased(MouseButton button)
	{
		return mouseButtonStates[(int)button] == KeyState::RELEASED;
	}

	const glm::vec2& InputManager::GetMousePosition()
	{
		return mousePosition;
	}

	const glm::vec2& InputManager::GetMouseOffset()
	{
		return mouseOffset;
	}

	const glm::vec2& InputManager::GetMouseWheelPosition()
	{
		return mouseWheelPosition;
	}

	const glm::vec2& InputManager::GetMouseWheelOffset()
	{
		return mouseWheelOffset;
	}

	void InputManager::Update()
	{
		for (int i = 0; i < KEY_STATE_COUNT; i++)
		{
			if (keyStates[i] == KeyState::PRESSED && keysPressedThisFrame[i] == false)
				keyStates[i] = KeyState::DOWN;

			keysPressedThisFrame[i] = false;
		}

		for (int i = 0; i < MOUSE_BUTTON_COUNT; i++)
		{
			if (mouseButtonStates[i] == KeyState::PRESSED && mouseButtonsPressedThisFrame[i] == false)
				mouseButtonStates[i] = KeyState::DOWN;

			mouseButtonsPressedThisFrame[i] = false;
		}

		if (cursorReceivedThisFrame) cursorReceivedThisFrame = false;
		else mouseOffset.x = mouseOffset.y = 0;

		if (wheelReceivedThisFrame) wheelReceivedThisFrame = false;
		else mouseWheelOffset.x = mouseWheelOffset.y = 0;
	}
}