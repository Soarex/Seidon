#include "Extension.h"

namespace Seidon
{
	void Extension::Bind(const std::wstring& path)
	{
		this->path = path;
		handle = LoadLibrary(path.c_str());

		if (!handle)
		{
			std::cout << "Error while loading DLL ";
			std::wcout << path << std::endl;
			return;
		}

		DllInit = (DllFunction)GetProcAddress(handle, "ExtInit");
		if (!DllInit)
		{
			std::cout << "Error while loading init function from DLL ";
			std::wcout << path << std::endl;
		}

		DllDestroy = (DllFunction)GetProcAddress(handle, "ExtDestroy");
		if (!DllDestroy)
		{
			std::cout << "Error while loading destroy function from DLL ";
			std::wcout << path << std::endl;
		}
	}

	void Extension::Unbind()
	{
		this->path = L"";
		FreeLibrary(handle);

		DllInit = nullptr;
		DllDestroy = nullptr;
	}

	void Extension::Init()
	{
		if (!DllInit) return;
		DllInit(*Application::Get());
	}

	void Extension::Destroy()
	{
		if (!DllDestroy) return;
		DllDestroy(*Application::Get());
	}
}