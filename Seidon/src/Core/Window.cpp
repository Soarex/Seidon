#include "Window.h"
#include "../Debug/Debug.h"

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
    void APIENTRY glDebugOutput(GLenum source,
        GLenum type,
        unsigned int id,
        GLenum severity,
        GLsizei length,
        const char* message,
        const void* userParam)
    {
        // ignore non-significant error/warning codes
        if (id == 131169 || id == 131185 || id == 131218 || id == 131204) return;

        std::cout << "---------------" << std::endl;
        std::cout << "Debug message (" << id << "): " << message << std::endl;

        switch (source)
        {
        case GL_DEBUG_SOURCE_API:             std::cout << "Source: API"; break;
        case GL_DEBUG_SOURCE_WINDOW_SYSTEM:   std::cout << "Source: Window System"; break;
        case GL_DEBUG_SOURCE_SHADER_COMPILER: std::cout << "Source: Shader Compiler"; break;
        case GL_DEBUG_SOURCE_THIRD_PARTY:     std::cout << "Source: Third Party"; break;
        case GL_DEBUG_SOURCE_APPLICATION:     std::cout << "Source: Application"; break;
        case GL_DEBUG_SOURCE_OTHER:           std::cout << "Source: Other"; break;
        } std::cout << std::endl;

        switch (type)
        {
        case GL_DEBUG_TYPE_ERROR:               std::cout << "Type: Error"; break;
        case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: std::cout << "Type: Deprecated Behaviour"; break;
        case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:  std::cout << "Type: Undefined Behaviour"; break;
        case GL_DEBUG_TYPE_PORTABILITY:         std::cout << "Type: Portability"; break;
        case GL_DEBUG_TYPE_PERFORMANCE:         std::cout << "Type: Performance"; break;
        case GL_DEBUG_TYPE_MARKER:              std::cout << "Type: Marker"; break;
        case GL_DEBUG_TYPE_PUSH_GROUP:          std::cout << "Type: Push Group"; break;
        case GL_DEBUG_TYPE_POP_GROUP:           std::cout << "Type: Pop Group"; break;
        case GL_DEBUG_TYPE_OTHER:               std::cout << "Type: Other"; break;
        } std::cout << std::endl;

        switch (severity)
        {
        case GL_DEBUG_SEVERITY_HIGH:         std::cout << "Severity: high"; break;
        case GL_DEBUG_SEVERITY_MEDIUM:       std::cout << "Severity: medium"; break;
        case GL_DEBUG_SEVERITY_LOW:          std::cout << "Severity: low"; break;
        case GL_DEBUG_SEVERITY_NOTIFICATION: std::cout << "Severity: notification"; break;
        } std::cout << std::endl;
        std::cout << std::endl;
    }

    void Window::Init(const std::string& name, unsigned int width, unsigned int height)
    {
        SD_ASSERT(!initialized, "Trying to init already initialized window");

        instance = this;

        this->name = name;
        this->width = width;
        this->height = height;

        deltaTime = 0;
        lastFrameTime = 0;
        mouseCursorEnabled = true;
        fullscreenEnabled = false;

        glfwInit();
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        
        handle = glfwCreateWindow(width, height, name.c_str(), NULL, NULL);

        if (handle == NULL)
        {
            std::cerr << "Failed to create GLFW window" << std::endl;
            glfwTerminate();
        }
        glfwMakeContextCurrent(handle);
        glfwSwapInterval(0);

        SD_ASSERT(gladLoadGLLoader((GLADloadproc)glfwGetProcAddress), "Failed to initialize GLAD");

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
        ImGui_ImplOpenGL3_Init("#version 460");
        SetImGuiStyle();

        initialized = true;

#ifndef NDEBUG
        glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);
        int flags = 0; 
        glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
        //if (flags & GL_CONTEXT_FLAG_DEBUG_BIT)
        {
            glEnable(GL_DEBUG_OUTPUT);
            glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
            glDebugMessageCallback(glDebugOutput, nullptr);
            glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
        }
#endif
    }

    void Window::Destroy()
    {
        SD_ASSERT(initialized, "Attempting to destroy not initialized Window");

        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();

        glfwDestroyWindow(handle);
        glfwTerminate();

        initialized = false;
    }

    void Window::BeginFrame()
    {
        SD_ASSERT(initialized, "Window not initialized");

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
    }

    float Window::EndFrame()
    {
        SD_ASSERT(initialized, "Window not initialized");

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

    void Window::SetImGuiStyle()
    {
        ImGuiStyle& style = ImGui::GetStyle();
        style.WindowBorderSize = 0.0f;
        style.WindowMenuButtonPosition = ImGuiDir_None;
        style.FramePadding = ImVec2(6.0f, 6.0f);
        style.ItemSpacing = ImVec2(10.0f, 10.0f);
        style.TabRounding = 0;

        ImVec4 backgroundColor = ImVec4{ 0.2f, 0.2f, 0.2f, 0.5f };
        ImVec4 activeBackgroundColor = ImVec4{ 0.3f, 0.3f, 0.3f, 0.5f };
        ImVec4 accentColor = ImVec4{ 0.0f, 0.38f, 0.85f, 0.7f };
        ImVec4 highlightColor = ImVec4{ 0.0f, 0.46f, 1.0f, 0.7f };

        ImGui::PushStyleColor(ImGuiCol_Border, backgroundColor);
        ImGui::PushStyleColor(ImGuiCol_MenuBarBg, 0);

        ImGui::PushStyleColor(ImGuiCol_FrameBg, backgroundColor);
        ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, highlightColor);
        ImGui::PushStyleColor(ImGuiCol_FrameBgActive, accentColor);

        ImGui::PushStyleColor(ImGuiCol_TitleBg, backgroundColor);
        ImGui::PushStyleColor(ImGuiCol_TitleBgCollapsed, backgroundColor);
        ImGui::PushStyleColor(ImGuiCol_TitleBgActive, activeBackgroundColor);

        ImGui::PushStyleColor(ImGuiCol_Button, backgroundColor);
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, highlightColor);
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, accentColor);

        ImGui::PushStyleColor(ImGuiCol_Header, backgroundColor);
        ImGui::PushStyleColor(ImGuiCol_HeaderHovered, highlightColor);
        ImGui::PushStyleColor(ImGuiCol_HeaderActive, accentColor);

        ImGui::PushStyleColor(ImGuiCol_Tab, 0);
        ImGui::PushStyleColor(ImGuiCol_TabHovered, highlightColor);
        ImGui::PushStyleColor(ImGuiCol_TabActive, ImVec4{ 0.08f, 0.08f, 0.08f, 0.94f });
        ImGui::PushStyleColor(ImGuiCol_TabUnfocused, 0);
        ImGui::PushStyleColor(ImGuiCol_TabUnfocusedActive, 0);

        ImGui::PushStyleColor(ImGuiCol_SeparatorHovered, highlightColor);
        ImGui::PushStyleColor(ImGuiCol_SeparatorActive, accentColor);

        ImGui::PushStyleColor(ImGuiCol_SliderGrab, highlightColor);
        ImGui::PushStyleColor(ImGuiCol_SliderGrabActive, accentColor);

        ImGui::PushStyleColor(ImGuiCol_DockingPreview, accentColor);
        ImGui::PushStyleColor(ImGuiCol_NavHighlight, accentColor);
        ImGui::PushStyleColor(ImGuiCol_CheckMark, accentColor);
        ImGui::PushStyleColor(ImGuiCol_TextSelectedBg, accentColor);
        ImGui::PushStyleColor(ImGuiCol_DragDropTarget, accentColor);
    }
}