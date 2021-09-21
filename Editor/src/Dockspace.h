#pragma once
#include <Seidon.h>

namespace Seidon
{
	class Dockspace
	{
	private:
		bool open;
		ImGuiWindowFlags windowFlags;
	public:
		Dockspace();
		void Begin();
		void End();
	};
}