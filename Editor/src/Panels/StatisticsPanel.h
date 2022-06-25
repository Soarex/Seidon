#pragma once
#include "Panel.h"

namespace Seidon
{
	class StatisticsPanel : public Panel
	{
	public:
		StatisticsPanel(Editor& editor);

		void Init() override;
		void Draw() override;
		void Destroy() override;
	};
}