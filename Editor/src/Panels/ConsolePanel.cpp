#include "ConsolePanel.h"
#include "../Editor.h"

namespace Seidon
{
	void ConsolePanel::Init()
	{
		
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
		ImGui::Text(editor.errorStream.str().c_str());
		ImGui::PopStyleColor();

		ImGui::PushStyleColor(ImGuiCol_Text, infoTextColor);
		ImGui::Text(editor.outStream.str().c_str());
		ImGui::PopStyleColor();
		ImGui::End();
	}

	void ConsolePanel::Clear()
	{
		editor.outStream.str("");
		editor.outStream.clear();

		editor.errorStream.str("");
		editor.errorStream.clear();
	}

	void ConsolePanel::Destroy()
	{
		
	}

}