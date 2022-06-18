#pragma once
#include "Panel.h"

#include <Seidon.h>

namespace Seidon
{
	class ViewportPanel : public Panel
	{
	public:
		ViewportPanel(Editor& editor) : Panel(editor) {}

		void Init() override;
		void Draw() override;
		void Destroy() override;

	private:
		BoundingBox viewportBounds;

	private:
		void ProcessInput();
	};
}