#pragma once
#include <glm/glm.hpp>
#include "Window.h"

#define GET_KEYCODE(x) GLFW_KEY_##x


namespace Seidon
{
	enum class KeyState
	{
		RELEASED = 0,
		PRESSED = 1,
		DOWN = 2
	};

	enum class MouseButton
	{
		LEFT = 0,
		RIGHT = 1,
		MIDDLE = 2
	};

	class InputManager
	{
	private:
		static constexpr unsigned int KEY_STATE_COUNT = 1024;
		static constexpr unsigned int MOUSE_BUTTON_COUNT = 3;

		static glm::vec2 mousePosition;
		static glm::vec2 mouseOffset;
		static glm::vec2 mouseWheelPosition;
		static glm::vec2 mouseWheelOffset;
		static bool cursorReceivedThisFrame;
		static bool wheelReceivedThisFrame;

		static KeyState keyStates[KEY_STATE_COUNT];
		static KeyState mouseButtonStates[MOUSE_BUTTON_COUNT];
		static bool keysPressedThisFrame[KEY_STATE_COUNT];
		static bool mouseButtonsPressedThisFrame[MOUSE_BUTTON_COUNT];
	public:
		static void Init();

		static bool GetKey(int keyCode);
		static bool GetKeyPressed(int keyCode);
		static bool GetKeyDown(int keyCode);
		static bool GetKeyReleased(int keyCode);

		static bool GetMouseButton(MouseButton button);
		static bool GetMouseButtonPressed(MouseButton button);
		static bool GetMouseButtonDown(MouseButton button);
		static bool GetMouseButtonReleased(MouseButton button);

		static const glm::vec2& GetMousePosition();
		static const glm::vec2& GetMouseOffset();

		static const glm::vec2& GetMouseWheelPosition();
		static const glm::vec2& GetMouseWheelOffset();

		static void Update();
	};
}