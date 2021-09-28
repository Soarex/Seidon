#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <Imgui/imgui.h>
#include <Imgui/imgui_impl_glfw.h>
#include <Imgui/imgui_impl_opengl3.h>

#include <string>
#include <iostream>
#include <functional>

namespace Seidon
{
	class Window
	{
	private:
		static std::string name;
		static GLFWwindow* handle;

		static float deltaTime, lastFrameTime;
		static unsigned int width, height;
		static bool mouseCursorEnabled, fullscreenEnabled;

		static std::vector<std::function<void(int, int)>>		windowSizeCallbacks;
		static std::vector<std::function<void(float, float)>>	cursorCallbacks;
		static std::vector<std::function<void(int, int)>>		mouseButtonCallbacks;
		static std::vector<std::function<void(float, float)>>	mouseWheelCallbacks;
		static std::vector<std::function<void(int, int)>>		keyboardCallbacks;

	public:
		static void Init(const std::string& name, unsigned int width = 800, unsigned int height = 600);
		static void Destroy();

		static void BeginFrame();
		static float EndFrame();
		static void EnableMouseCursor(bool value);
		static void EnableFullscreen(bool value);
		 
		static void ToggleMouseCursor();
		static void ToggleFullscreen();

		inline static bool ShouldClose() { return glfwWindowShouldClose(handle); }
		inline static void Close() { glfwSetWindowShouldClose(handle, true); }

		inline static void SetSize(unsigned int width, unsigned int height) { Window::width = width; Window::height = height;  glfwSetWindowSize(handle, width, height); }
		inline static void SetName(std::string name) { glfwSetWindowTitle(handle, name.c_str()); }

		inline static int GetWidth() { return width; }
		inline static int GetHeight() { return height; }
		inline static float GetDeltaTime() { return deltaTime; }
		inline static GLFWwindow* GetHandle() { return handle; }

		inline static void AddWindowSizeCallback(const std::function<void(int, int)>& callback) { windowSizeCallbacks.push_back(callback); }
		inline static void AddCursorCallback(const std::function<void(float, float)>& callback) { cursorCallbacks.push_back(callback); }
		inline static void AddMouseButtonCallback(const std::function<void(int, int)>& callback) { mouseButtonCallbacks.push_back(callback); }
		inline static void AddMouseWheelCallback(const std::function<void(float, float)>& callback) { mouseWheelCallbacks.push_back(callback); }
		inline static void AddKeyboardCallback(const std::function<void(int, int)>& callback) { keyboardCallbacks.push_back(callback); }

		friend void WindowSizeCallback(GLFWwindow* window, int width, int height);
		friend void CursorCallback(GLFWwindow* window, double xpos, double ypos);
		friend void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
		friend void MouseWheelCallback(GLFWwindow* window, double xpos, double ypos);
		friend void KeyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
	};
}