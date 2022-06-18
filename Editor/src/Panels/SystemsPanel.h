#pragma once
#include "Panel.h"

#include <Seidon.h>

namespace Seidon
{
	class SystemsPanel : public Panel
	{
	public:
		SystemsPanel(Editor& editor) : Panel(editor) {}

		void Init() override;
		void Draw() override;
	private:
		void DrawSystems(Scene* scene);
	};
}