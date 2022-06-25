#include "Extension.h"

#include "../Utils/StringUtils.h"
#include "../Debug/Debug.h"

namespace Seidon
{
	void Extension::Load(const std::string& path)
	{
		SD_ASSERT(!initialized, "Extension already initialized");

		this->path = path;
		this->name = GetNameFromPath(path);

		std::wstring widePath;
		widePath.assign(path.begin(), path.end());

		handle = LoadLibrary(widePath.c_str());

		if (!handle)
		{
			std::cout << "Error while loading DLL " << path << std::endl;
			return;
		}

		DllInit = (DllFunction)GetProcAddress(handle, "ExtInit");
		
		SD_ASSERT(DllInit, "Couldn't load extension function");

		DllDestroy = (DllFunction)GetProcAddress(handle, "ExtDestroy");
		
		SD_ASSERT(DllDestroy, "Couldn't load extension function");

		Init();
	}


	void Extension::Init()
	{
		SD_ASSERT(!initialized, "Extension already initialized");

		DllInit(*Application::Get());

		initialized = true;
	}

	void Extension::Destroy()
	{
		SD_ASSERT(initialized, "Extension not initialized");

		DllDestroy(*Application::Get());

		this->path = "";
		FreeLibrary(handle);

		DllInit = nullptr;
		DllDestroy = nullptr;

		initialized = false;
	}
}