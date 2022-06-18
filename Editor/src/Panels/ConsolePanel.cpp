#include "ConsolePanel.h"

namespace Seidon
{
	void ConsolePanel::Init()
	{
		oldOutStream = std::cout.rdbuf();
		//std::cout.rdbuf(outStream.rdbuf());

		oldErrorStream = std::cerr.rdbuf();
		//std::cerr.rdbuf(errorStream.rdbuf());
	}

	void ConsolePanel::Draw()
	{
		if (!ImGui::Begin("Console"))
		{
			ImGui::End();
			return;
		}
		
		if (ImGui::Button("Clear")) Clear();

		ImGui::PushStyleColor(ImGuiCol_Text, errorTextColor);
		ImGui::Text(errorStream.str().c_str());
		ImGui::PopStyleColor();

		ImGui::PushStyleColor(ImGuiCol_Text, infoTextColor);
		ImGui::Text(outStream.str().c_str());
		ImGui::PopStyleColor();
		ImGui::End();
	}

	void ConsolePanel::Clear()
	{
		outStream.str("");
		outStream.clear();

		errorStream.str("");
		errorStream.clear();
	}

	void ConsolePanel::Destroy()
	{
		std::cout.rdbuf(oldOutStream);
	}

}