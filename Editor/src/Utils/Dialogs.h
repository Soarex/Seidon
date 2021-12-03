#pragma once

#define GLFW_EXPOSE_NATIVE_WIN32

#include <Seidon.h>

#include <Windows.h>
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

#include <string>

std::string SaveFile(const char* filter)
{
    OPENFILENAMEA ofn;
    CHAR szFile[260] = { 0 };
    CHAR currentDir[256] = { 0 };

    ZeroMemory(&ofn, sizeof(OPENFILENAME));

    ofn.lStructSize = sizeof(OPENFILENAME);
    ofn.hwndOwner = glfwGetWin32Window((GLFWwindow*)Seidon::Application::Get()->GetWindow()->GetHandle());
    ofn.lpstrFile = szFile;
    ofn.nMaxFile = sizeof(szFile);

    if (GetCurrentDirectoryA(256, currentDir))
        ofn.lpstrInitialDir = currentDir;

    ofn.lpstrFilter = filter;
    ofn.nFilterIndex = 1;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT | OFN_NOCHANGEDIR;
    ofn.lpstrDefExt = strchr(filter, '\0') + 1;

    if (GetSaveFileNameA(&ofn) == TRUE)
        return ofn.lpstrFile;

    return std::string();
}

std::string LoadFile(const char* filter)
{
    OPENFILENAMEA ofn;
    CHAR szFile[260] = { 0 };
    CHAR currentDir[256] = { 0 };

    ZeroMemory(&ofn, sizeof(OPENFILENAME));

    ofn.lStructSize = sizeof(OPENFILENAME);
    ofn.hwndOwner = glfwGetWin32Window((GLFWwindow*)Seidon::Application::Get()->GetWindow()->GetHandle());
    ofn.lpstrFile = szFile;
    ofn.nMaxFile = sizeof(szFile);

    if (GetCurrentDirectoryA(256, currentDir))
        ofn.lpstrInitialDir = currentDir;

    ofn.lpstrFilter = filter;
    ofn.nFilterIndex = 1;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT | OFN_NOCHANGEDIR;
    ofn.lpstrDefExt = strchr(filter, '\0') + 1;

    if (GetOpenFileNameA(&ofn) == TRUE)
        return ofn.lpstrFile;

    return std::string();
}