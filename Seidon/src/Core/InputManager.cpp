#include "InputManager.h"

namespace Seidon
{
	void InputManager::Init(Window* window)
	{
		listenToCursor = true;

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
				if (!listenToCursor) return;

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
		if (blockInput) return false;

		return keyStates[keycode] == KeyState::PRESSED || keyStates[keycode] == KeyState::DOWN;
	}

	bool InputManager::GetKeyPressed(int keycode)
	{
		if (blockInput) return false;

		return keyStates[keycode] == KeyState::PRESSED;
	}

	bool InputManager::GetKeyDown(int keycode)
	{
		if (blockInput) return false;

		return keyStates[keycode] == KeyState::DOWN;
	}

	bool InputManager::GetKeyReleased(int keycode)
	{
		if (blockInput) return false;

		return keyStates[keycode] == KeyState::RELEASED;
	}

	bool InputManager::IsGamepadConnected(int index)
	{
		return glfwJoystickIsGamepad(GLFW_JOYSTICK_1 + index);
	}

	int InputManager::GetGamepadButton(int buttonCode)
	{
		return gamepadState.buttons[buttonCode];
	}

	float InputManager::GetGamepadAxis(int axisCode)
	{
		float epsilon = 0.1f;

		if (std::abs(gamepadState.axes[axisCode]) > epsilon)
			return gamepadState.axes[axisCode];
		else
			return 0;
	}

	bool InputManager::GetMouseButton(MouseButton button)
	{
		if (blockInput) return false;

		return mouseButtonStates[(int)button] == KeyState::PRESSED || mouseButtonStates[(int)button] == KeyState::DOWN;
	}

	bool InputManager::GetMouseButtonPressed(MouseButton button)
	{
		if (blockInput) return false;

		return mouseButtonStates[(int)button] == KeyState::PRESSED;
	}

	bool InputManager::GetMouseButtonDown(MouseButton button)
	{
		if (blockInput) return false;

		return mouseButtonStates[(int)button] == KeyState::DOWN;
	}

	bool InputManager::GetMouseButtonReleased(MouseButton button)
	{
		if (blockInput) return false;

		return mouseButtonStates[(int)button] == KeyState::RELEASED;
	}

	const glm::vec2& InputManager::GetMousePosition()
	{
		if (blockInput) return { 0, 0 };

		return mousePosition;
	}

	const glm::vec2& InputManager::GetMouseOffset()
	{
		if (blockInput) return { 0, 0 };

		return mouseOffset;
	}

	const glm::vec2& InputManager::GetMouseWheelPosition()
	{
		if (blockInput) return { 0, 0 };

		return mouseWheelPosition;
	}

	const glm::vec2& InputManager::GetMouseWheelOffset()
	{
		if (blockInput) return { 0, 0 };

		return mouseWheelOffset;
	}

	void InputManager::SetMousePosition(const glm::vec2& position)
	{
		mouseOffset.x = position.x - mousePosition.x;
		mouseOffset.y = mousePosition.y - position.y;

		mousePosition.x = position.x;
		mousePosition.y = position.y;

		cursorReceivedThisFrame = true;
	}

	void InputManager::Update()
	{
		if (glfwJoystickIsGamepad(GLFW_JOYSTICK_1))
			glfwGetGamepadState(GLFW_JOYSTICK_1, &gamepadState);

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