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
		bool initialized = false;
		std::string name;
		GLFWwindow* handle;

		float deltaTime, lastFrameTime;
		unsigned int width, height;
		bool mouseCursorEnabled, fullscreenEnabled;

		std::list<std::function<void(int, int)>>		windowSizeCallbacks;
		std::list<std::function<void(float, float)>>	cursorCallbacks;
		std::list<std::function<void(int, int)>>		mouseButtonCallbacks;
		std::list<std::function<void(float, float)>>	mouseWheelCallbacks;
		std::list<std::function<void(int, int)>>		keyboardCallbacks;

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

		inline std::list<std::function<void(int, int)>>::iterator AddWindowSizeCallback(const std::function<void(int, int)>& callback) { windowSizeCallbacks.push_back(callback); auto it = windowSizeCallbacks.end(); return --it; }
		inline std::list<std::function<void(float, float)>>::iterator AddCursorCallback(const std::function<void(float, float)>& callback) { cursorCallbacks.push_back(callback); auto it = cursorCallbacks.end(); return --it;  }
		inline std::list<std::function<void(int, int)>>::iterator AddMouseButtonCallback(const std::function<void(int, int)>& callback) { mouseButtonCallbacks.push_back(callback); auto it = mouseButtonCallbacks.end(); return --it;}
		inline std::list<std::function<void(float, float)>>::iterator AddMouseWheelCallback(const std::function<void(float, float)>& callback) { mouseWheelCallbacks.push_back(callback); auto it = mouseWheelCallbacks.end(); return --it;}
		inline std::list<std::function<void(int, int)>>::iterator AddKeyboardCallback(const std::function<void(int, int)>& callback) { keyboardCallbacks.push_back(callback); auto it = keyboardCallbacks.end(); return --it;}

		inline void removeWindowSizeCallback(std::list<std::function<void(int, int)>>::iterator& position) { windowSizeCallbacks.erase(position); }
		inline void removeCursorCallback(std::list<std::function<void(float, float)>>::iterator& position) { cursorCallbacks.erase(position); }
		inline void removeMouseButtonCallback(std::list<std::function<void(int, int)>>::iterator& position) { mouseButtonCallbacks.erase(position); }
		inline void removeMouseWheelCallback(std::list<std::function<void(float, float)>>::iterator& position) { mouseWheelCallbacks.erase(position); }
		inline void removeKeyboardCallback(std::list<std::function<void(int, int)>>::iterator& position) { keyboardCallbacks.erase(position); }

		friend void WindowSizeCallback(GLFWwindow* window, int width, int height);
		friend void CursorCallback(GLFWwindow* window, double xpos, double ypos);
		friend void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
		friend void MouseWheelCallback(GLFWwindow* window, double xpos, double ypos);
		friend void KeyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mods);

	private:
		void SetImGuiStyle();
	};
}