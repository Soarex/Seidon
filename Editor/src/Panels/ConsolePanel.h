#pragma once
#include "Panel.h"

#include <Seidon.h>
#include <sstream>

namespace Seidon
{
	class ConsolePanel : public Panel
	{
	public:
		ConsolePanel(Editor& editor) : Panel(editor) {}

		void Init() override;
		void Draw() override;
		void Destroy() override;

		void Clear();
	private:
		static constexpr int errorTextColor = 0xff0000ff;
		static constexpr int warningTextColor = 0xffff00ff;
		static constexpr int infoTextColor = 0xffffffff;

		std::ostringstream outStream;
		std::streambuf* oldOutStream;

		std::ostringstream errorStream;
		std::streambuf* oldErrorStream;
	};
}