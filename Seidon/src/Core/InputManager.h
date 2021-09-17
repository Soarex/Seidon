#pragma once
#include <glm/glm.hpp>
#include "Window.h"

#define GET_KEYCODE(x) GLFW_KEY_##x

enum class KeyState
{
	RELEASED = 0,
	PRESSED = 1,
	DOWN = 2
};

namespace Seidon
{
	class InputManager
	{
	private:
		static constexpr unsigned int KEY_STATE_COUNT = 1024;

		static glm::vec2 mousePosition;
		static glm::vec2 mouseOffset;
		static bool receivedThisFrame;

		static KeyState keyStates[KEY_STATE_COUNT];
		static bool pressedThisFrame[KEY_STATE_COUNT];
	public:
		static void Init();

		static bool GetKey(int keyCode);
		static bool GetKeyPressed(int keyCode);
		static bool GetKeyDown(int keyCode);
		static bool GetKeyReleased(int keyCode);

		static const glm::vec2& GetMousePosition();
		static const glm::vec2& GetMouseOffset();

		static void Update();
	};
}