#include "DrawFunctions.h"

#include "../Editor.h"

#include <Imgui/imgui_stdlib.h>

namespace Seidon
{
	ChangeStatus DrawVec2Control(const std::string& label, glm::vec2& values, float resetValue, glm::vec2* oldValue)
	{
		static std::unordered_map<glm::vec2*, glm::vec2> oldValues;

		ImGuiIO& io = ImGui::GetIO();
		auto boldFont = io.Fonts->Fonts[0];

		bool updateOldValue = false;
		bool changed = false;
		bool stoppedChanging = false;

		glm::vec2 old = values;

		ImGui::PushID(label.c_str());

		float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
		ImVec2 buttonSize = { lineHeight + 3.0f, lineHeight };

		ImGuiWindowFlags flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
		ImGui::BeginChild("Text", ImVec2(ImGui::GetContentRegionAvail().x * 0.33f, buttonSize.y), false, flags);

		ImGui::AlignTextToFramePadding();
		ImGui::Text(label.c_str());

		ImGui::EndChild();

		ImGui::SameLine();
		ImGui::BeginChild("Controls", ImVec2(ImGui::GetContentRegionAvail().x, buttonSize.y), false, flags);
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, 5 });

		float fieldWidth = ImGui::GetContentRegionAvail().x / 2 - buttonSize.x;

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.9f, 0.2f, 0.2f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
		ImGui::PushFont(boldFont);
		if (ImGui::Button("X", buttonSize))
		{
			values.x = resetValue;
			changed = true;
			stoppedChanging = true;
			updateOldValue = true;
		}

		ImGui::PopFont();
		ImGui::PopStyleColor(3);

		ImGui::SameLine();

		ImGui::PushItemWidth(fieldWidth);

		changed |= ImGui::DragFloat("##X", &values.x, 0.1f, 0.0f, 0.0f, "%.2f");
		if (ImGui::IsItemDeactivatedAfterEdit()) stoppedChanging = true;
		if (ImGui::IsItemActivated()) updateOldValue = true;

		ImGui::PopItemWidth();
		ImGui::SameLine();

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.3f, 0.8f, 0.3f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
		ImGui::PushFont(boldFont);
		if (ImGui::Button("Y", buttonSize))
		{
			values.y = resetValue;
			changed = true;
			stoppedChanging = true;
			updateOldValue = true;
		}
		ImGui::PopFont();
		ImGui::PopStyleColor(3);

		ImGui::SameLine();

		ImGui::PushItemWidth(fieldWidth);

		changed |= ImGui::DragFloat("##Y", &values.y, 0.1f, 0.0f, 0.0f, "%.2f");
		if (ImGui::IsItemDeactivatedAfterEdit()) stoppedChanging = true;
		if (ImGui::IsItemActivated()) updateOldValue = true;

		ImGui::PopItemWidth();

		ImGui::PopStyleVar();
		ImGui::EndChild();

		ImGui::PopID();

		if (oldValue) *oldValue = oldValues[&values];

		if (stoppedChanging) return ChangeStatus::CHANGED;
		if (changed) return ChangeStatus::CHANGING;

		if (updateOldValue) oldValues[&values] = old;

		return ChangeStatus::NO_CHANGE;
	}

	ChangeStatus DrawVec3Control(const std::string& label, glm::vec3& values, float resetValue, glm::vec3* oldValue)
	{
		static std::unordered_map<glm::vec3*, glm::vec3> oldValues;

		ImGuiIO& io = ImGui::GetIO();
		auto boldFont = io.Fonts->Fonts[0];

		bool updateOldValue = false;
		bool changed = false;
		bool stoppedChanging = false;

		glm::vec3 old = values;

		ImGui::PushID(label.c_str());

		float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
		ImVec2 buttonSize = { lineHeight + 3.0f, lineHeight };

		ImGuiWindowFlags flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
		ImGui::BeginChild("Text", ImVec2(ImGui::GetContentRegionAvail().x * 0.33f, buttonSize.y), false, flags);

		ImGui::AlignTextToFramePadding();
		ImGui::Text(label.c_str());

		ImGui::EndChild();

		ImGui::SameLine();
		ImGui::BeginChild("Controls", ImVec2(ImGui::GetContentRegionAvail().x, buttonSize.y), false, flags);
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, 5 });

		float fieldWidth = ImGui::GetContentRegionAvail().x / 3 - buttonSize.x;

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.9f, 0.2f, 0.2f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
		ImGui::PushFont(boldFont);
		if (ImGui::Button("X", buttonSize))
		{
			values.x = resetValue;
			changed = true;
			stoppedChanging = true;
			updateOldValue = true;
		}

		ImGui::PopFont();
		ImGui::PopStyleColor(3);

		ImGui::SameLine();

		ImGui::PushItemWidth(fieldWidth);

		changed |= ImGui::DragFloat("##X", &values.x, 0.1f, 0.0f, 0.0f, "%.2f");
		if (ImGui::IsItemDeactivatedAfterEdit()) stoppedChanging = true;
		if (ImGui::IsItemActivated()) updateOldValue = true;

		ImGui::PopItemWidth();
		ImGui::SameLine();

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.3f, 0.8f, 0.3f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
		ImGui::PushFont(boldFont);
		if (ImGui::Button("Y", buttonSize))
		{
			values.y = resetValue;
			changed = true;
			stoppedChanging = true;
			updateOldValue = true;
		}
		ImGui::PopFont();
		ImGui::PopStyleColor(3);

		ImGui::SameLine();

		ImGui::PushItemWidth(fieldWidth);

		changed |= ImGui::DragFloat("##Y", &values.y, 0.1f, 0.0f, 0.0f, "%.2f");
		if (ImGui::IsItemDeactivatedAfterEdit()) stoppedChanging = true;
		if (ImGui::IsItemActivated()) updateOldValue = true;

		ImGui::PopItemWidth();
		ImGui::SameLine();

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.2f, 0.35f, 0.9f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
		ImGui::PushFont(boldFont);
		if (ImGui::Button("Z", buttonSize))
		{
			values.z = resetValue;
			changed = true;
			stoppedChanging = true;
			updateOldValue = true;
		}
		ImGui::PopFont();
		ImGui::PopStyleColor(3);

		ImGui::SameLine();

		ImGui::PushItemWidth(fieldWidth);
		
		changed |= ImGui::DragFloat("##Z", &values.z, 0.1f, 0.0f, 0.0f, "%.2f");
		if (ImGui::IsItemDeactivatedAfterEdit()) stoppedChanging = true;
		if (ImGui::IsItemActivated()) updateOldValue = true;

		ImGui::PopItemWidth();

		ImGui::PopStyleVar();
		ImGui::EndChild();

		ImGui::PopID();

		if (oldValue) *oldValue = oldValues[&values];

		if (stoppedChanging) return ChangeStatus::CHANGED;
		if (changed) return ChangeStatus::CHANGING;

		if(updateOldValue) oldValues[&values] = old;

		return ChangeStatus::NO_CHANGE;
	}

	ChangeStatus DrawColorControl(const std::string& label, glm::vec3& values, float resetValue, glm::vec3* oldValue)
	{
		static std::unordered_map<glm::vec3*, glm::vec3> oldValues;

		ImGuiIO& io = ImGui::GetIO();
		auto boldFont = io.Fonts->Fonts[0];
		int i[3] = { IM_F32_TO_INT8_UNBOUND(values.x), IM_F32_TO_INT8_UNBOUND(values.y), IM_F32_TO_INT8_UNBOUND(values.z)};

		bool updateOldValue = false;
		bool changed = false;
		bool stoppedChanging = false;

		glm::vec3 old = values;

		ImGui::PushID(label.c_str());

		ImGuiWindowFlags flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
		float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
		ImGui::BeginChild("Text", ImVec2(ImGui::GetContentRegionAvail().x * 0.33f, lineHeight), false, flags);

		ImGui::AlignTextToFramePadding();
		ImGui::Text(label.c_str());
		
		ImGui::EndChild();

		ImGui::SameLine();
		ImGui::BeginChild("Controls", ImVec2(ImGui::GetContentRegionAvail().x, lineHeight), false, flags);

		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, 5 });

		ImVec2 buttonSize = { lineHeight + 3.0f, lineHeight };
		float fieldWidth = ImGui::GetContentRegionAvail().x / 3 - buttonSize.x * 4.0f / 3.0f;

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.9f, 0.2f, 0.2f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
		ImGui::PushFont(boldFont);
		if (ImGui::Button("R", buttonSize))
		{
			i[0] = 0;
			changed = true;
			stoppedChanging = true;
			updateOldValue = true;
		}
		ImGui::PopFont();
		ImGui::PopStyleColor(3);

		ImGui::SameLine();

		ImGui::PushItemWidth(fieldWidth);

		changed |= ImGui::DragInt("##R", &i[0], 1.0f, 0.0f, 255, "%3d");
		values.x = i[0] / 255.0f;

		if (ImGui::IsItemDeactivatedAfterEdit()) stoppedChanging = true;
		if (ImGui::IsItemActivated()) updateOldValue = true;

		ImGui::PopItemWidth();
		ImGui::SameLine();

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.3f, 0.8f, 0.3f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
		ImGui::PushFont(boldFont);
		if (ImGui::Button("G", buttonSize))
		{
			i[1] = 0;
			changed = true;
			stoppedChanging = true;
			updateOldValue = true;
		}
		ImGui::PopFont();
		ImGui::PopStyleColor(3);

		ImGui::SameLine();

		ImGui::PushItemWidth(fieldWidth);

		changed |= ImGui::DragInt("##G", &i[1], 1.0f, 0.0f, 255, "%3d");
		values.y = i[1] / 255.0f;

		if (ImGui::IsItemDeactivatedAfterEdit()) stoppedChanging = true;
		if (ImGui::IsItemActivated()) updateOldValue = true;

		ImGui::PopItemWidth();
		ImGui::SameLine();

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.2f, 0.35f, 0.9f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
		ImGui::PushFont(boldFont);
		if (ImGui::Button("B", buttonSize))
		{
			i[2] = 0;
			changed = true;
			stoppedChanging = true;
			updateOldValue = true;
		}
		ImGui::PopFont();
		ImGui::PopStyleColor(3);

		ImGui::SameLine();

		ImGui::PushItemWidth(fieldWidth);

		changed |= ImGui::DragInt("##B", &i[2], 1.0f, 0.0f, 255, "%3d");
		values.z = i[2] / 255.0f;

		if (ImGui::IsItemDeactivatedAfterEdit()) stoppedChanging = true;
		if (ImGui::IsItemActivated()) updateOldValue = true;

		ImGui::PopItemWidth();
		ImGui::SameLine();

		if (ImGui::ColorButton("##ColorButton", { values.x, values.y, values.z, 1.0f }, NULL, buttonSize))
			ImGui::OpenPopup("ColorPicker");

		if (ImGui::BeginPopup("ColorPicker"))
		{
			ImGuiColorEditFlags picker_flags_to_forward = ImGuiColorEditFlags_DataTypeMask_ | ImGuiColorEditFlags_PickerMask_ | ImGuiColorEditFlags_InputMask_ | ImGuiColorEditFlags_HDR | ImGuiColorEditFlags_NoAlpha | ImGuiColorEditFlags_AlphaBar;
			ImGuiColorEditFlags picker_flags = ImGuiColorEditFlags_DisplayMask_ | ImGuiColorEditFlags_NoLabel | ImGuiColorEditFlags_AlphaPreviewHalf;
			changed |= ImGui::ColorPicker3("##ColorPicker", (float*)&values, picker_flags);

			if (ImGui::IsItemDeactivatedAfterEdit()) stoppedChanging = true;
			if (ImGui::IsItemActivated()) updateOldValue = true;

			ImGui::EndPopup();
		}

		ImGui::PopStyleVar();
		
		ImGui::EndChild();

		ImGui::PopID();

		if (oldValue) *oldValue = oldValues[&values];

		if (stoppedChanging) return ChangeStatus::CHANGED;
		if (changed) return ChangeStatus::CHANGING;

		if (updateOldValue) oldValues[&values] = old;

		return ChangeStatus::NO_CHANGE;
	}

	ChangeStatus DrawIdControl(const std::string& label, UUID& value, UUID* oldValue)
	{
		ImGui::PushID(label.c_str());

		bool changed = false;

		float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
		ImGuiWindowFlags flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
		ImGui::BeginChild("Text", ImVec2(ImGui::GetContentRegionAvail().x * 0.33f, lineHeight), false, flags);

		ImGui::AlignTextToFramePadding();
		ImGui::Text(label.c_str());
		
		ImGui::EndChild();

		ImGui::SameLine();

		ImGui::BeginChild("Control", ImVec2(ImGui::GetContentRegionAvail().x, lineHeight), false, flags);

		ImGui::PushItemWidth(-1);

		std::string s = std::to_string(value);
		ImGui::InputText("##X", &s, ImGuiInputTextFlags_ReadOnly);

		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("HIERARCHY_PANEL_ENTITY"))
			{
				Entity& entity = *(Entity*)payload->Data;

				if(oldValue) *oldValue = value;
				value = entity.GetId();

				changed = true;
			}

			ImGui::EndDragDropTarget();
		}

		ImGui::PopItemWidth();

		ImGui::EndChild();

		ImGui::PopID();

		if (changed) return ChangeStatus::CHANGED;

		return ChangeStatus::NO_CHANGE;
	}

	ChangeStatus DrawFloatControl(const std::string& label, float& value, float* oldValue)
	{
		static std::unordered_map<float*, float> oldValues;

		ImGui::PushID(label.c_str());

		ImGuiWindowFlags flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
		float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
		ImGui::BeginChild("Text", ImVec2(ImGui::GetContentRegionAvail().x * 0.33f, lineHeight), false, flags);

		ImGui::AlignTextToFramePadding();
		ImGui::Text(label.c_str());

		ImGui::EndChild();

		ImGui::SameLine();

		ImGui::BeginChild("Control", ImVec2(ImGui::GetContentRegionAvail().x, lineHeight), false, flags);

		ImGui::PushItemWidth(-1);
		float old = value;

		bool changed = ImGui::DragFloat("##X", &value, 0.1f, 0.0f, 0.0f, "%.2f");
		bool stoppedChanging = false;

		if (ImGui::IsItemDeactivatedAfterEdit()) stoppedChanging = true;
		if (ImGui::IsItemActivated()) oldValues[&value] = old;


		ImGui::PopItemWidth();
		ImGui::EndChild();

		ImGui::PopID();

		if (oldValue) *oldValue = oldValues[&value];

		if (stoppedChanging) return ChangeStatus::CHANGED;
		if (changed) return ChangeStatus::CHANGING;

		return ChangeStatus::NO_CHANGE;
	}

	ChangeStatus DrawFloatSliderControl(const std::string& label, float& value, float min, float max, float* oldValue)
	{
		static std::unordered_map<float*, float> oldValues;

		ImGui::PushID(label.c_str());

		ImGuiWindowFlags flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
		float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
		ImGui::BeginChild("Text", ImVec2(ImGui::GetContentRegionAvail().x * 0.33f, lineHeight), false, flags);

		ImGui::AlignTextToFramePadding();
		ImGui::Text(label.c_str());
		
		ImGui::EndChild();

		ImGui::SameLine();

		ImGui::BeginChild("Control", ImVec2(ImGui::GetContentRegionAvail().x, lineHeight), false, flags);

		ImGui::PushItemWidth(-1);

		float old = value;
		bool changed = ImGui::SliderFloat("##X", &value, min, max, "%.2f");

		bool stoppedChanging = false;
		if (ImGui::IsItemDeactivatedAfterEdit()) stoppedChanging = true;
		if (ImGui::IsItemActivated()) oldValues[&value] = old;

		ImGui::PopItemWidth();

		ImGui::EndChild();

		ImGui::PopID();

		if (oldValue) *oldValue = oldValues[&value];

		if (stoppedChanging) return ChangeStatus::CHANGED;
		if (changed) return ChangeStatus::CHANGING;

		return ChangeStatus::NO_CHANGE;
	}

	ChangeStatus DrawBoolControl(const std::string& label, bool& value, bool* oldValue)
	{
		ImGui::PushID(label.c_str());

		ImGuiWindowFlags flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
		float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
		ImGui::BeginChild("Text", ImVec2(ImGui::GetContentRegionAvail().x * 0.33f, lineHeight), false, flags);

		ImGui::AlignTextToFramePadding();
		ImGui::Text(label.c_str());
		
		ImGui::EndChild();

		ImGui::SameLine();

		ImGui::BeginChild("Control", ImVec2(ImGui::GetContentRegionAvail().x, lineHeight), false, flags);

		ImGui::PushItemWidth(-1);

		if (oldValue) *oldValue = value;
		bool changed = ImGui::Checkbox("##bool", &value);

		ImGui::PopItemWidth();

		ImGui::EndChild();

		ImGui::PopID();

		if (changed) return ChangeStatus::CHANGED;

		return ChangeStatus::NO_CHANGE;
	}

	ChangeStatus DrawIntControl(const std::string& label, int& value, int* oldValue)
	{
		static std::unordered_map<int*, int> oldValues;

		ImGui::PushID(label.c_str());

		ImGuiWindowFlags flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
		float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
		ImGui::BeginChild("Text", ImVec2(ImGui::GetContentRegionAvail().x * 0.33f, lineHeight), false, flags);

		ImGui::AlignTextToFramePadding();
		ImGui::Text(label.c_str());
		
		ImGui::EndChild();

		ImGui::SameLine();

		ImGui::BeginChild("Control", ImVec2(ImGui::GetContentRegionAvail().x, lineHeight), false, flags);

		ImGui::PushItemWidth(-1);

		float old = value;

		bool changed = ImGui::DragInt("##X", &value);
		bool stoppedChanging = false;

		if (ImGui::IsItemDeactivatedAfterEdit()) stoppedChanging = true;
		if (ImGui::IsItemActivated()) oldValues[&value] = old;

		ImGui::PopItemWidth();

		ImGui::EndChild();

		ImGui::PopID();

		if (oldValue) *oldValue = oldValues[&value];

		if (stoppedChanging) return ChangeStatus::CHANGED;
		if (changed) return ChangeStatus::CHANGING;

		return ChangeStatus::NO_CHANGE;
	}

	ChangeStatus DrawStringControl(const std::string& label, std::string& value, std::string* oldValue)
	{
		static std::unordered_map<std::string*, std::string> oldValues;
		std::string old = value;

		ImGui::PushID(label.c_str());

		ImGuiWindowFlags flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
		float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
		ImGui::BeginChild("Text", ImVec2(ImGui::GetContentRegionAvail().x * 0.33f, lineHeight), false, flags);

		ImGui::AlignTextToFramePadding();
		ImGui::Text(label.c_str());
		
		ImGui::EndChild();

		ImGui::SameLine();

		ImGui::BeginChild("Control", ImVec2(ImGui::GetContentRegionAvail().x, lineHeight), false, flags);

		ImGui::PushItemWidth(-1);

		bool stoppedChanging = false;
		bool changed = ImGui::InputText("##string", &value, ImGuiInputTextFlags_CtrlEnterForNewLine);

		if (ImGui::IsItemDeactivatedAfterEdit()) stoppedChanging = true;
		if (ImGui::IsItemActivated()) oldValues[&value] = old;

		ImGui::PopItemWidth();

		ImGui::EndChild();

		ImGui::PopID();

		if (oldValue) *oldValue = oldValues[&value];

		if (stoppedChanging) return ChangeStatus::CHANGED;
		if (changed) return ChangeStatus::CHANGING;

		return ChangeStatus::NO_CHANGE;
	}

	ChangeStatus DrawTextureControl(const std::string& label, Texture** texture, float size, Texture** oldValue)
	{
		bool changed = false;
		ImGui::PushID(label.c_str());

		ImGui::AlignTextToFramePadding();
		ImGui::Text(label.c_str());

		ImGuiWindowFlags flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
		ImGui::BeginChild("Image", ImVec2(ImGui::GetContentRegionAvail().x * 0.33f, size), false, flags);

		ImGui::Image((ImTextureID)(*texture)->GetRenderId(), ImVec2{ size, size }, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });
		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("FILE_BROWSER_TEXTURE"))
			{
				ResourceManager& resourceManager = *Application::Get()->GetResourceManager();
				std::string path = (const char*)payload->Data;
				
				if (oldValue) *oldValue = *texture;
				*texture = resourceManager.GetOrLoadAsset<Texture>(path);

				changed = true;
			}

			ImGui::EndDragDropTarget();
		}

		ImGui::EndChild();

		ImGui::SameLine();

		ImGui::BeginChild("Text", ImVec2(ImGui::GetContentRegionAvail().x, size), false, flags);

		std::filesystem::path path = std::string((*texture)->path);
		ImGui::Text(path.filename().string().c_str());

		ImGui::EndChild();
		ImGui::PopID();

		if (changed) return ChangeStatus::CHANGED;

		return ChangeStatus::NO_CHANGE;
	}

	ChangeStatus DrawCubemapControl(const std::string& label, HdrCubemap** cubemap, float size, HdrCubemap** oldValue)
	{
		ResourceManager& resourceManager = ((Editor*)Application::Get())->editorResourceManager;
		bool changed = false;

		ImGui::PushID(label.c_str());

		ImGui::AlignTextToFramePadding();
		ImGui::Text(label.c_str());

		ImGuiWindowFlags flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
		ImGui::BeginChild("Image", ImVec2(ImGui::GetContentRegionAvail().x * 0.33f, size), false, flags);

		ImGui::Image((ImTextureID)resourceManager.GetOrLoadAsset<Texture>("Resources/FileIcon.sdtex", true)->GetRenderId(), ImVec2{ size, size }, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });

		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("FILE_BROWSER_CUBEMAP"))
			{
				ResourceManager& resourceManager = *Application::Get()->GetResourceManager();
				std::string path = (const char*)payload->Data;

				if (oldValue) *oldValue = *cubemap;
				*cubemap = resourceManager.GetOrLoadAsset<HdrCubemap>(path);

				changed = true;
			}

			ImGui::EndDragDropTarget();
		}

		ImGui::EndChild();

		ImGui::SameLine();

		ImGui::BeginChild("Text", ImVec2(ImGui::GetContentRegionAvail().x, size), false, flags);

		std::filesystem::path path = std::string((*cubemap)->GetPath());
		ImGui::Text(path.filename().string().c_str());
		
		ImGui::EndChild();
		ImGui::PopID();

		if (changed) return ChangeStatus::CHANGED;

		return ChangeStatus::NO_CHANGE;
	}

	ChangeStatus DrawMeshControl(const std::string& label, Mesh** mesh, float size, Mesh** oldValue)
	{
		ResourceManager& resourceManager = ((Editor*)Application::Get())->editorResourceManager;
		bool changed = false;

		ImGui::PushID(label.c_str());

		ImGui::AlignTextToFramePadding();
		ImGui::Text(label.c_str());

		ImGuiWindowFlags flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
		ImGui::BeginChild("Image", ImVec2(ImGui::GetContentRegionAvail().x * 0.33f, size), false, flags);

		ImGui::Image((ImTextureID)resourceManager.GetOrLoadAsset<Texture>("Resources/ModelIcon.sdtex", true)->GetRenderId(), ImVec2{ size, size }, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });

		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("FILE_BROWSER_MESH"))
			{
				ResourceManager& resourceManager = *Application::Get()->GetResourceManager();
				std::string path = (const char*)payload->Data;

				if (oldValue) *oldValue = *mesh;
				*mesh = resourceManager.GetOrLoadAsset<Mesh>(path);

				changed = true;
			}

			ImGui::EndDragDropTarget();
		}

		ImGui::EndChild();

		ImGui::SameLine();

		ImGui::BeginChild("Text", ImVec2(ImGui::GetContentRegionAvail().x, size), false, flags);

		ImGui::Text((*mesh)->name.c_str());
		
		ImGui::EndChild();

		ImGui::PopID();

		if (changed) return ChangeStatus::CHANGED;

		return ChangeStatus::NO_CHANGE;
	}

	ChangeStatus DrawSkinnedMeshControl(const std::string& label, SkinnedMesh** mesh, float size, SkinnedMesh** oldValue)
	{
		ResourceManager& resourceManager = ((Editor*)Application::Get())->editorResourceManager;
		bool changed = false;

		ImGui::PushID(label.c_str());

		ImGui::AlignTextToFramePadding();
		ImGui::Text(label.c_str());

		ImGuiWindowFlags flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
		ImGui::BeginChild("Image", ImVec2(ImGui::GetContentRegionAvail().x * 0.33f, size), false, flags);

		ImGui::Image((ImTextureID)resourceManager.GetOrLoadAsset<Texture>("Resources/SkinnedMeshIcon.sdtex", true)->GetRenderId(), ImVec2{ size, size }, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });

		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("FILE_BROWSER_SKINNED_MESH"))
			{
				ResourceManager& resourceManager = *Application::Get()->GetResourceManager();
				std::string path = (const char*)payload->Data;

				if (oldValue) *oldValue = *mesh;
				*mesh = resourceManager.GetOrLoadAsset<SkinnedMesh>(path);

				changed = true;
			}

			ImGui::EndDragDropTarget();
		}

		ImGui::EndChild();

		ImGui::SameLine();

		ImGui::BeginChild("Text", ImVec2(ImGui::GetContentRegionAvail().x, size), false, flags);

		ImGui::Text((*mesh)->name.c_str());

		ImGui::EndChild();

		ImGui::PopID();

		if (changed) return ChangeStatus::CHANGED;

		return ChangeStatus::NO_CHANGE;
	}

	ChangeStatus DrawMaterialControl(const std::string& label, Material** material, float size, bool* clicked, Material** oldValue)
	{
		ResourceManager& resourceManager = ((Editor*)Application::Get())->editorResourceManager;
		static Material* selectedMaterial = nullptr;

		bool changed = false;

		ImGui::PushID(label.c_str());

		ImGui::AlignTextToFramePadding();
		ImGui::Text(label.c_str());

		ImGuiWindowFlags flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
		ImGui::BeginChild("Image", ImVec2(ImGui::GetContentRegionAvail().x * 0.33f, size), false, flags);

		ImGui::Image((ImTextureID)resourceManager.GetOrLoadAsset<Texture>("Resources/MaterialIcon.sdtex", true)->GetRenderId(), ImVec2{ size, size }, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });
		
		if (ImGui::IsItemClicked() && clicked)
			*clicked = true;

		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("FILE_BROWSER_MATERIAL"))
			{
				ResourceManager& resourceManager = *Application::Get()->GetResourceManager();
				std::string path = (const char*)payload->Data;

				if (oldValue) *oldValue = *material;
				*material = resourceManager.GetOrLoadAsset<Material>(path);

				changed = true;
			}

			ImGui::EndDragDropTarget();
		}

		ImGui::EndChild();
		ImGui::SameLine();
		ImGui::BeginChild("Text", ImVec2(ImGui::GetContentRegionAvail().x, size), false, flags);

		ImGui::Text((*material)->name.c_str());

		ImGui::EndChild();

		ImGui::PopID();
		
		if (changed) return ChangeStatus::CHANGED;

		return ChangeStatus::NO_CHANGE;
	}

	ChangeStatus DrawMaterialEditor(const std::string& label, Material* material, bool* open)
	{
		ResourceManager& resourceManager = *Application::Get()->GetResourceManager();
		ChangeStatus changeStatus = ChangeStatus::NO_CHANGE;

		if (ImGui::Begin(label.c_str(), open))
		{
			if (ChangeStatus res = DrawStringControl("Name", material->name); (int)res)
				changeStatus = res;
			
			if ((int)DrawShaderControl("Shader", &material->shader))
			{
				memset(material->data, 0, 500);

				for (MemberData& m : material->shader->GetBufferLayout()->members)
					switch (m.type)
					{
					case Types::VECTOR2:
						*(glm::vec2*)(material->data + m.offset) = glm::vec2(1);
						break;

					case Types::VECTOR3_COLOR: 
						*(glm::vec3*)(material->data + m.offset) = glm::vec3(1);
						break;

					case Types::TEXTURE:
						*(Texture**)(material->data + m.offset) = resourceManager.GetOrLoadAsset<Texture>("albedo_default", true);
						break;
					}

				changeStatus = ChangeStatus::CHANGED;
			}

			if (ChangeData res = DrawMetaType(material->data, *material->shader->GetBufferLayout()); (int)res.status)
				changeStatus = res.status;

			ImGuiStyle& style = ImGui::GetStyle();
			float size = ImGui::CalcTextSize("Save").x + style.FramePadding.x * 2.0f;

			float offset = (ImGui::GetContentRegionAvail().x - size) * 0.5;
			ImGui::SetCursorPosX(ImGui::GetCursorPosX() + offset);
			if (ImGui::Button("Save"))
			{
				std::string relativePath = Application::Get()->GetResourceManager()->GetAssetPath(material->id);
				material->Save(Application::Get()->GetResourceManager()->RelativeToAbsolutePath(relativePath));
			}
		
			}
			ImGui::End();

			return changeStatus;
	}

	ChangeStatus DrawAnimationControl(const std::string& label, Animation** animation, float size, Animation** oldValue)
	{
		ResourceManager& resourceManager = ((Editor*)Application::Get())->editorResourceManager;
		bool changed = false;
		ImGui::PushID(label.c_str());

		ImGui::AlignTextToFramePadding();
		ImGui::Text(label.c_str());

		ImGuiWindowFlags flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
		ImGui::BeginChild("Image", ImVec2(ImGui::GetContentRegionAvail().x * 0.33f, size), false, flags);

		ImGui::Image((ImTextureID)resourceManager.GetOrLoadAsset<Texture>("Resources/AnimationIcon.sdtex", true)->GetRenderId(), ImVec2{ size, size }, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });

		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("FILE_BROWSER_ANIMATION"))
			{
				ResourceManager& resourceManager = *Application::Get()->GetResourceManager();
				std::string path = (const char*)payload->Data;

				if (oldValue) *oldValue = *animation;
				*animation = resourceManager.GetOrLoadAsset<Animation>(path);

				changed = true;
			}

			ImGui::EndDragDropTarget();
		}

		ImGui::EndChild();
		ImGui::SameLine();
		ImGui::BeginChild("Text", ImVec2(ImGui::GetContentRegionAvail().x, size), false, flags);

		ImGui::Text((*animation)->name.c_str());

		ImGui::EndChild();

		ImGui::PopID();

		if (changed) return ChangeStatus::CHANGED;

		return ChangeStatus::NO_CHANGE;
	}

	ChangeStatus DrawShaderControl(const std::string& label, Shader** shader, float size, Shader** oldValue)
	{
		ResourceManager& resourceManager = ((Editor*)Application::Get())->editorResourceManager;
		bool changed = false;

		ImGui::PushID(label.c_str());

		ImGui::AlignTextToFramePadding();
		ImGui::Text(label.c_str());

		ImGuiWindowFlags flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
		ImGui::BeginChild("Image", ImVec2(ImGui::GetContentRegionAvail().x * 0.33f, size), false, flags);

		ImGui::Image((ImTextureID)resourceManager.GetOrLoadAsset<Texture>("Resources/FileIcon.sdtex", true)->GetRenderId(), ImVec2{ size, size }, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });

		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("FILE_BROWSER_SHADER"))
			{
				ResourceManager& resourceManager = *Application::Get()->GetResourceManager();
				std::string path = (const char*)payload->Data;

				if (oldValue) *oldValue = *shader;
				*shader = resourceManager.GetOrLoadAsset<Shader>(path);
				changed = true;
			}

			ImGui::EndDragDropTarget();
		}

		ImGui::EndChild();

		ImGui::SameLine();

		ImGui::BeginChild("Text", ImVec2(ImGui::GetContentRegionAvail().x, size), false, flags);

		ImGui::Text((*shader)->name.c_str());

		ImGui::EndChild();

		ImGui::PopID();

		if (changed) return ChangeStatus::CHANGED;

		return ChangeStatus::NO_CHANGE;
	}

	ChangeStatus DrawFontControl(const std::string& label, Font** font, float size, Font** oldValue)
	{
		ResourceManager& resourceManager = ((Editor*)Application::Get())->editorResourceManager;
		bool changed = false;

		ImGui::PushID(label.c_str());

		ImGui::AlignTextToFramePadding();
		ImGui::Text(label.c_str());

		ImGuiWindowFlags flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
		ImGui::BeginChild("Image", ImVec2(ImGui::GetContentRegionAvail().x * 0.33f, size), false, flags);

		ImGui::Image((ImTextureID)resourceManager.GetOrLoadAsset<Texture>("Resources/FontIcon.sdtex", true)->GetRenderId(), ImVec2{ size, size }, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });

		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("FILE_BROWSER_FONT"))
			{
				ResourceManager& resourceManager = *Application::Get()->GetResourceManager();
				std::string path = (const char*)payload->Data;

				if (oldValue) *oldValue = *font;

				*font = resourceManager.GetOrLoadAsset<Font>(path);

				changed = true;
			}

			ImGui::EndDragDropTarget();
		}

		ImGui::EndChild();
		ImGui::SameLine();
		ImGui::BeginChild("Text", ImVec2(ImGui::GetContentRegionAvail().x, size), false, flags);

		if(*font) ImGui::Text((*font)->GetName().c_str());

		ImGui::EndChild();
		ImGui::PopID();

		if (changed) return ChangeStatus::CHANGED;

		return ChangeStatus::NO_CHANGE;
	}

	ChangeStatus DrawMeshColliderControl(const std::string& label, MeshCollider** collider, float size, MeshCollider** oldValue)
	{
		ResourceManager& resourceManager = ((Editor*)Application::Get())->editorResourceManager;
		bool changed = false;

		ImGui::PushID(label.c_str());

		ImGui::AlignTextToFramePadding();
		ImGui::Text(label.c_str());

		ImGuiWindowFlags flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
		ImGui::BeginChild("Image", ImVec2(ImGui::GetContentRegionAvail().x * 0.33f, size), false, flags);

		ImGui::Image((ImTextureID)resourceManager.GetOrLoadAsset<Texture>("Resources/MeshColliderIcon.sdtex", true)->GetRenderId(), ImVec2{ size, size }, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });

		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("FILE_BROWSER_COLLIDER"))
			{
				ResourceManager& resourceManager = *Application::Get()->GetResourceManager();
				std::string path = (const char*)payload->Data;

				if (oldValue) *oldValue = *collider;

				*collider = resourceManager.GetOrLoadAsset<MeshCollider>(path);

				changed = true;
			}

			ImGui::EndDragDropTarget();
		}

		ImGui::EndChild();
		ImGui::SameLine();
		ImGui::BeginChild("Text", ImVec2(ImGui::GetContentRegionAvail().x, size), false, flags);

		if (*collider) ImGui::Text((*collider)->name.c_str());

		ImGui::EndChild();

		ImGui::PopID();

		if (changed) return ChangeStatus::CHANGED;

		return ChangeStatus::NO_CHANGE;
	}

	ChangeStatus DrawSoundControl(const std::string& label, Sound** sound, float size, Sound** oldValue)
	{
		ResourceManager& resourceManager = ((Editor*)Application::Get())->editorResourceManager;
		bool changed = false;

		ImGui::PushID(label.c_str());

		ImGui::AlignTextToFramePadding();
		ImGui::Text(label.c_str());

		ImGuiWindowFlags flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
		ImGui::BeginChild("Image", ImVec2(ImGui::GetContentRegionAvail().x * 0.33f, size), false, flags);

		ImGui::Image((ImTextureID)resourceManager.GetOrLoadAsset<Texture>("Resources/SoundIcon.sdtex", true)->GetRenderId(), ImVec2{ size, size }, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });

		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("FILE_BROWSER_SOUND"))
			{
				ResourceManager& resourceManager = *Application::Get()->GetResourceManager();
				std::string path = (const char*)payload->Data;

				if (oldValue) *oldValue = *sound;

				*sound = resourceManager.GetOrLoadAsset<Sound>(path);

				changed = true;
			}

			ImGui::EndDragDropTarget();
		}

		ImGui::EndChild();
		ImGui::SameLine();
		ImGui::BeginChild("Text", ImVec2(ImGui::GetContentRegionAvail().x, size), false, flags);

		if (*sound) ImGui::Text((*sound)->name.c_str());

		ImGui::EndChild();
		ImGui::PopID();

		if (changed) return ChangeStatus::CHANGED;

		return ChangeStatus::NO_CHANGE;
	}

	ChangeData DrawReflectedMember(void* object, MemberData& member)
	{
		ChangeData data;
		data.status = ChangeStatus::NO_CHANGE;
		data.modifiedMember = member;

		byte* obj = (byte*) object;
		
		if (member.type == Types::ID)
		{
			UUID old;
			data.status = DrawIdControl(member.name.c_str(), *(UUID*)(obj + member.offset), &old);
			memcpy(data.oldValue, &old, sizeof(UUID));
			memcpy(data.newValue, (UUID*)(obj + member.offset), sizeof(UUID));
		}

		if (member.type == Types::FLOAT)
		{
			float old;
			data.status = DrawFloatControl(member.name.c_str(), *(float*)(obj + member.offset), &old);
			memcpy(data.oldValue, &old, sizeof(float));
			memcpy(data.newValue, (float*)(obj + member.offset), sizeof(float));
		}

		if (member.type == Types::INT)
		{
			int old;
			data.status = DrawIntControl(member.name.c_str(), *(int*)(obj + member.offset), &old);
			memcpy(data.oldValue, &old, sizeof(int));
			memcpy(data.newValue, (int*)(obj + member.offset), sizeof(int));
		}

		if (member.type == Types::FLOAT_ANGLE)
		{
			float old;

			float temp = glm::degrees(*(float*)(obj + member.offset));
			data.status = DrawFloatControl(member.name.c_str(), temp, &old);
			*(float*)(obj + member.offset) = glm::radians(temp);

			old = glm::radians(old);

			memcpy(data.oldValue, &old, sizeof(float));
			memcpy(data.newValue, (float*)(obj + member.offset), sizeof(float));
		}

		if (member.type == Types::FLOAT_NORMALIZED)
		{
			float old;
			data.status = DrawFloatSliderControl(member.name.c_str(), *(float*)(obj + member.offset), 0.0f, 1.0f, &old);

			memcpy(data.oldValue, &old, sizeof(float));
			memcpy(data.newValue, (float*)(obj + member.offset), sizeof(float));
		}

		if (member.type == Types::BOOL)
		{
			bool old;
			data.status = DrawBoolControl(member.name.c_str(), *(bool*)(obj + member.offset), &old);

			memcpy(data.oldValue, &old, sizeof(bool));
			memcpy(data.newValue, (float*)(obj + member.offset), sizeof(bool));
		}

		if (member.type == Types::STRING)
		{
			std::string old;
			data.status = DrawStringControl(member.name.c_str(), *(std::string*)(obj + member.offset), &old);

			int size = old.size() + 1;
			memcpy(data.oldValue, &size, sizeof(int));
			memcpy(data.oldValue + sizeof(int), old.c_str(), size);

			size = ((std::string*)(obj + member.offset))->size() + 1;
			memcpy(data.newValue, &size, sizeof(int));
			memcpy(data.newValue + sizeof(int), ((std::string*)(obj + member.offset))->c_str(), size);
		}

		if (member.type == Types::VECTOR2)
		{
			glm::vec2 old;
			data.status = DrawVec2Control(member.name.c_str(), *(glm::vec2*)(obj + member.offset), 0.0f, &old);

			memcpy(data.oldValue, &old, sizeof(glm::vec2));
			memcpy(data.newValue, (glm::vec2*)(obj + member.offset), sizeof(glm::vec2));
		}

		if (member.type == Types::VECTOR3)
		{
			glm::vec3 old;
			data.status = DrawVec3Control(member.name.c_str(), *(glm::vec3*)(obj + member.offset), 0.0f, &old);

			memcpy(data.oldValue, &old, sizeof(glm::vec3));
			memcpy(data.newValue, (glm::vec3*)(obj + member.offset), sizeof(glm::vec3));
		}

		if (member.type == Types::VECTOR3_ANGLES)
		{
			glm::vec3 old;

			glm::vec3 temp = glm::degrees(*(glm::vec3*)(obj + member.offset));
			data.status = DrawVec3Control(member.name.c_str(), temp, 0.0f, &old);
			*(glm::vec3*)(obj + member.offset) = glm::radians(temp);

			old = glm::radians(old);

			memcpy(data.oldValue, &old, sizeof(glm::vec3));
			memcpy(data.newValue, (glm::vec3*)(obj + member.offset), sizeof(glm::vec3));
		}

		if (member.type == Types::VECTOR3_COLOR)
		{
			glm::vec3 old;
			data.status = DrawColorControl(member.name.c_str(), *(glm::vec3*)(obj + member.offset), 0.0f, &old);

			memcpy(data.oldValue, &old, sizeof(glm::vec3));
			memcpy(data.newValue, (glm::vec3*)(obj + member.offset), sizeof(glm::vec3));
		}

		if (member.type == Types::TEXTURE)
		{
			Texture** t = (Texture**)(obj + member.offset);
			Texture* old;

			data.status = DrawTextureControl(member.name.c_str(), t, 64.0f, &old);

			memcpy(data.oldValue, &old, sizeof(Texture*));
			memcpy(data.newValue, *t, sizeof(Texture*));
		}

		if (member.type == Types::MESH)
		{
			Mesh** m = (Mesh**)(obj + member.offset);
			Mesh* old;

			data.status = DrawMeshControl(member.name.c_str(), m, 64.0f, &old);

			memcpy(data.oldValue, &old, sizeof(Mesh*));
			memcpy(data.newValue, *m, sizeof(Mesh*));
		}

		if (member.type == Types::SKINNED_MESH)
		{
			SkinnedMesh** m = (SkinnedMesh**)(obj + member.offset);
			SkinnedMesh* old;

			data.status = DrawSkinnedMeshControl(member.name.c_str(), m, 64.0f, &old);

			memcpy(data.oldValue, &old, sizeof(SkinnedMesh*));
			memcpy(data.newValue, *m, sizeof(SkinnedMesh*));
		}

		if (member.type == Types::MATERIAL)
		{
			Material** m = (Material**)(obj + member.offset);
			static bool open;
			bool clicked = false;

			DrawMaterialControl(member.name.c_str(), m, 64, &clicked);

			if (clicked)
				open = !open;
			
			if(open)
				data.status = DrawMaterialEditor("Material Editor", *m, &open);
		}

		if (member.type == Types::MATERIAL_VECTOR)
		{
			std::vector<Material*>* v = (std::vector<Material*>*)(obj + member.offset);

			static bool open;
			static int selected;

			for (int i = 0; i < v->size(); i++)
			{
				bool clicked = false;

				DrawMaterialControl(member.name.c_str(), &v->at(i), 64, &clicked);
				if (clicked)
				{
					if (selected == i && open)
						open = false;
					else
					{
						selected = i;
						open = true;
					}
				}
			}

			if (open && selected < v->size())
				data.status = DrawMaterialEditor("Material Editor", v->at(selected), &open);
		}

		if (member.type == Types::CUBEMAP)
		{
			HdrCubemap** c = (HdrCubemap**)(obj + member.offset);
			HdrCubemap* old;

			data.status = DrawCubemapControl(member.name.c_str(), c, 64.0f, &old);

			memcpy(data.oldValue, &old, sizeof(HdrCubemap*));
			memcpy(data.newValue, *c, sizeof(HdrCubemap*));
		}

		if (member.type == Types::ANIMATION)
		{
			Animation** a = (Animation**)(obj + member.offset);
			Animation* old;

			data.status = DrawAnimationControl(member.name.c_str(), a, 64.0f, &old);

			memcpy(data.oldValue, &old, sizeof(Animation*));
			memcpy(data.newValue, *a, sizeof(Animation*));
		}

		if (member.type == Types::FONT)
		{
			Font** f = (Font**)(obj + member.offset);
			Font* old;

			data.status = DrawFontControl(member.name.c_str(), f, 64.0f, &old);

			memcpy(data.oldValue, &old, sizeof(Font*));
			if(*f)memcpy(data.newValue, *f, sizeof(Font*));
		}

		if (member.type == Types::MESH_COLLIDER)
		{
			MeshCollider** c = (MeshCollider**)(obj + member.offset);
			MeshCollider* old;

			data.status = DrawMeshColliderControl(member.name.c_str(), c, 64.0f, &old);

			memcpy(data.oldValue, &old, sizeof(MeshCollider*));
			if (*c)memcpy(data.newValue, *c, sizeof(MeshCollider*));
		}

		if (member.type == Types::SOUND)
		{
			Sound** s = (Sound**)(obj + member.offset);
			Sound* old;

			data.status = DrawSoundControl(member.name.c_str(), s, 64.0f, &old);

			memcpy(data.oldValue, &old, sizeof(Sound*));
			if (*s)memcpy(data.newValue, *s, sizeof(Sound*));
		}

		return data;
	}

	ChangeData DrawMetaType(void* object, MetaType& type)
	{
		ChangeData res;
		res.status = ChangeStatus::NO_CHANGE;

		for (MemberData& member : type.members)
		{
			ChangeData changeData;
			changeData = DrawReflectedMember(object, member);

			if ((int)changeData.status) res = changeData;
		}

		if ((int)res.status && type.OnChange) type.OnChange(object);

		return res;
	}
}