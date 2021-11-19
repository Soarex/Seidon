#pragma once
#include <Seidon.h>

namespace Seidon
{
	class SystemsPanel
	{
	public:
		SystemsPanel() = default;

		void Init();
		void Draw();

	private:
		void DrawSystems(Scene* scene);
	};
}