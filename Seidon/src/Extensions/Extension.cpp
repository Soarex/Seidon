#include "Extension.h"
#include "../Debug/Debug.h"

namespace Seidon
{
	void Extension::Bind(const std::wstring& path)
	{
		SD_ASSERT(!bound, "Extension already bound");

		this->path = path;
		handle = LoadLibrary(path.c_str());

		
		if (!handle)
		{
			std::cout << "Error while loading DLL ";
			std::wcout << path << std::endl;
			return;
		}

		DllInit = (DllFunction)GetProcAddress(handle, "ExtInit");
		
		SD_ASSERT(DllInit, "Couldn't load extension function");

		DllDestroy = (DllFunction)GetProcAddress(handle, "ExtDestroy");
		
		SD_ASSERT(DllDestroy, "Couldn't load extension function");

		bound = true;
	}

	void Extension::Unbind()
	{
		SD_ASSERT(bound, "Extension not bound");

		this->path = L"";
		FreeLibrary(handle);

		DllInit = nullptr;
		DllDestroy = nullptr;

		bound = false;
	}

	void Extension::Init()
	{
		SD_ASSERT(!initialized, "Extension already initialized");
		SD_ASSERT(bound, "Extension not bound");

		DllInit(*Application::Get());

		initialized = true;
	}

	void Extension::Destroy()
	{
		SD_ASSERT(initialized, "Extension not initialized");
		SD_ASSERT(bound, "Extension not bound"); 

		DllDestroy(*Application::Get());
	}
}