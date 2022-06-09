#pragma once
#include <Seidon.h>

#include <sstream>
namespace Seidon
{
	class ConsolePanel
	{
	public:
		void Init();
		void Draw();
		void Destroy();

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