#pragma once

#include "core.h"

namespace Seidon {

	class SEIDON_API Application {
	public:
		Application();
		virtual ~Application();
		void Run();
	};

	Application* CreateApplication();
}
