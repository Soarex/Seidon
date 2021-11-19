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
		std::string name;
		GLFWwindow* handle;

		float deltaTime, lastFrameTime;
		unsigned int width, height;
		bool mouseCursorEnabled, fullscreenEnabled;

		std::vector<std::function<void(int, int)>>		windowSizeCallbacks;
		std::vector<std::function<void(float, float)>>	cursorCallbacks;
		std::vector<std::function<void(int, int)>>		mouseButtonCallbacks;
		std::vector<std::function<void(float, float)>>	mouseWheelCallbacks;
		std::vector<std::function<void(int, int)>>		keyboardCallbacks;

	public:
		void Init(const std::string& name, unsigned int width = 800, unsigned int height = 600);
		void Destroy();

		void BeginFrame();
		float EndFrame();
		void EnableMouseCursor(bool value);
		void EnableFullscreen(bool value);
		 
		void ToggleMouseCursor();
		void ToggleFullscreen();

		inline bool ShouldClose() { return glfwWindowShouldClose(handle); }
		inline void Close() { glfwSetWindowShouldClose(handle, true); }

		inline void SetSize(unsigned int width, unsigned int height) { Window::width = width; Window::height = height;  glfwSetWindowSize(handle, width, height); }
		inline void SetName(std::string name) { glfwSetWindowTitle(handle, name.c_str()); }
		void SetIcon(unsigned char* data, int width, int height);

		inline int GetWidth() { return width; }
		inline int GetHeight() { return height; }
		inline float GetDeltaTime() { return deltaTime; }
		inline GLFWwindow* GetHandle() { return handle; }

		inline void AddWindowSizeCallback(const std::function<void(int, int)>& callback) { windowSizeCallbacks.push_back(callback); }
		inline void AddCursorCallback(const std::function<void(float, float)>& callback) { cursorCallbacks.push_back(callback); }
		inline void AddMouseButtonCallback(const std::function<void(int, int)>& callback) { mouseButtonCallbacks.push_back(callback); }
		inline void AddMouseWheelCallback(const std::function<void(float, float)>& callback) { mouseWheelCallbacks.push_back(callback); }
		inline void AddKeyboardCallback(const std::function<void(int, int)>& callback) { keyboardCallbacks.push_back(callback); }

		friend void WindowSizeCallback(GLFWwindow* window, int width, int height);
		friend void CursorCallback(GLFWwindow* window, double xpos, double ypos);
		friend void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
		friend void MouseWheelCallback(GLFWwindow* window, double xpos, double ypos);
		friend void KeyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mods);

	private:
		void SetImGuiStyle();
	};
}