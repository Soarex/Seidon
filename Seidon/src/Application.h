#pragma once
#include "Core/Window.h"

namespace Seidon 
{
	class Application
	{
	public:
		Application();
		virtual ~Application();
		virtual void Run();
	};

	Application* CreateApplication();
}
