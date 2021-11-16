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

		glm::vec2 mousePosition;
		glm::vec2 mouseOffset;
		glm::vec2 mouseWheelPosition;
		glm::vec2 mouseWheelOffset;
		bool cursorReceivedThisFrame;
		bool wheelReceivedThisFrame;

		KeyState keyStates[KEY_STATE_COUNT];
		KeyState mouseButtonStates[MOUSE_BUTTON_COUNT];
		bool keysPressedThisFrame[KEY_STATE_COUNT];
		bool mouseButtonsPressedThisFrame[MOUSE_BUTTON_COUNT];

		bool blockInput;
	public:
		void Init(Window* window);
		void Update();

		bool GetKey(int keyCode);
		bool GetKeyPressed(int keyCode);
		bool GetKeyDown(int keyCode);
		bool GetKeyReleased(int keyCode);

		bool GetMouseButton(MouseButton button);
		bool GetMouseButtonPressed(MouseButton button);
		bool GetMouseButtonDown(MouseButton button);
		bool GetMouseButtonReleased(MouseButton button);

		const glm::vec2& GetMousePosition();
		const glm::vec2& GetMouseOffset();

		const glm::vec2& GetMouseWheelPosition();
		const glm::vec2& GetMouseWheelOffset();

		void BlockInput(bool value) { blockInput = value; };
	};
}