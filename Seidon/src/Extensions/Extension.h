#pragma once
#include "../Core/Asset.h"
#include "Core/Application.h"

#include <Windows.h>
#include <string>

namespace Seidon
{
	using DllFunction = void(*)(Application&);

	class Extension : public Asset
	{
	public:
		void Destroy();

		void Save(std::ofstream& out) {}
		void Load(std::ifstream& in)  {}

		void Save(const std::string& path) {}
		void Load(const std::string& path);

	private:
		bool initialized = false;

		HINSTANCE handle;

		DllFunction DllInit;
		DllFunction DllDestroy;
		
	private:
		void Init();
	};
}
