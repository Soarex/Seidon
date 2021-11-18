#include "Window.h"

namespace Seidon
{
    Window* instance;

    void WindowSizeCallback(GLFWwindow* window, int width, int height)
    {
        instance->width = width;
        instance->height = height;

        for (auto& callback : instance->windowSizeCallbacks)
            callback(width, height);
    }

    void CursorCallback(GLFWwindow* window, double xpos, double ypos)
    {
        for (auto& callback : instance->cursorCallbacks)
            callback(xpos, ypos);
    }

    void KeyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
    {
        for (auto& callback : instance->keyboardCallbacks)
            callback(key, action);
    }

    void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
    {
        for (auto& callback : instance->mouseButtonCallbacks)
            callback(button, action);
    }

    void MouseWheelCallback(GLFWwindow* window, double xpos, double ypos)
    {
        for (auto& callback : instance->mouseWheelCallbacks)
            callback(xpos, ypos);
    }


    void Window::Init(const std::string& name, unsigned int width, unsigned int height)
    {
        instance = this;

        this->name = name;
        this->width = width;
        this->height = height;

        deltaTime = 0;
        lastFrameTime = 0;
        mouseCursorEnabled = true;
        fullscreenEnabled = false;

        glfwInit();
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        handle = glfwCreateWindow(width, height, name.c_str(), NULL, NULL);

        if (handle == NULL)
        {
            std::cout << "Failed to create GLFW window" << std::endl;
            glfwTerminate();
        }

        glfwMakeContextCurrent(handle);

        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
            std::cout << "Failed to initialize GLAD" << std::endl;

        glfwSetWindowSizeCallback(handle, WindowSizeCallback);
        glfwSetKeyCallback(handle, KeyboardCallback);
        glfwSetCursorPosCallback(handle, CursorCallback);
        glfwSetScrollCallback(handle, MouseWheelCallback);
        glfwSetMouseButtonCallback(handle, MouseButtonCallback);

        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO(); (void)io;
        io.WantCaptureMouse = true;
        io.WantCaptureKeyboard = true;
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
        ImGui::StyleColorsDark();
        ImGui_ImplGlfw_InitForOpenGL(handle, true);
        ImGui_ImplOpenGL3_Init("#version 330");
    }

    void Window::Destroy()
    {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();

        glfwDestroyWindow(handle);
        glfwTerminate();
    }

    void Window::BeginFrame()
    {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
    }

    float Window::EndFrame()
    {
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(handle);
        glfwPollEvents();

        float currentFrameTime = glfwGetTime();
        deltaTime = currentFrameTime - lastFrameTime;
        lastFrameTime = currentFrameTime;


        return deltaTime;
    }

    void Window::EnableMouseCursor(bool value)
    {
        glfwSetInputMode(handle, GLFW_CURSOR, value ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED);
        mouseCursorEnabled = value;
    }

    void Window::EnableFullscreen(bool value)
    {
        const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());

        if (value)
            glfwSetWindowMonitor(handle, glfwGetPrimaryMonitor(), 0, 0, mode->width, mode->height, mode->refreshRate);
        else
            glfwSetWindowMonitor(handle, NULL, 400, 300, width, height, mode->refreshRate);

        fullscreenEnabled = value;
    }

    void Window::ToggleMouseCursor()
    {
        mouseCursorEnabled = !mouseCursorEnabled;
        EnableMouseCursor(mouseCursorEnabled);
    }

    void Window::ToggleFullscreen()
    {
        fullscreenEnabled = !fullscreenEnabled;
        EnableFullscreen(fullscreenEnabled);
    }

    void Window::SetIcon(unsigned char* data, int width, int height)
    {
        GLFWimage image;
        image.pixels = data;
        image.height = height;
        image.width = width;
        
        glfwSetWindowIcon(handle, 1, &image);
    }
}