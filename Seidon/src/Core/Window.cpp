#include "Window.h"

namespace Seidon
{
    std::string Window::name;
    GLFWwindow* Window::handle;

    float Window::deltaTime;
    float Window::lastFrameTime;

    unsigned int Window::width;
    unsigned int Window::height;

    bool Window::mouseCursorEnabled = true;
    bool Window::fullscreenEnabled = false;

    std::vector<std::function<void(int, int)>>		Window::windowSizeCallbacks;
    std::vector<std::function<void(float, float)>>	Window::mouseCallbacks;
    std::vector<std::function<void(int, int)>>		Window::keyboardCallbacks;

    void WindowSizeCallback(GLFWwindow* window, int width, int height)
    {
        Window::width = width;
        Window::height = height;

        for (auto callback : Window::windowSizeCallbacks)
            callback(width, height);
    }

    void MouseCallback(GLFWwindow* window, double xpos, double ypos)
    {
        for (auto callback : Window::mouseCallbacks)
            callback(xpos, ypos);
    }

    void KeyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
    {
        for (auto callback : Window::keyboardCallbacks)
            callback(key, action);
    }

    void Window::Init(const std::string& name, unsigned int width, unsigned int height)
    {
        Window::name = name;
        Window::width = width;
        Window::height = height;

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
        glfwSetCursorPosCallback(handle, MouseCallback);

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
}