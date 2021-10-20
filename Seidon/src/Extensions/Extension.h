#pragma once
#include "Core/Application.h"

#include <Windows.h>
#include <string>

namespace Seidon
{
	using DllFunction = void(*)(Application&);

	class Extension
	{
	private:
		HINSTANCE handle;
		std::wstring path;

		DllFunction DllInit;
		DllFunction DllDestroy;

	public:
		void Bind(const std::wstring& path);
		void Unbind();

		void Init();
		void Destroy();
	};
}
