#include "DrawFunctions.h"

#include "../Editor.h"

namespace Seidon
{
	void DrawVec3Control(const std::string& label, glm::vec3& values, float resetValue, float columnWidth)
	{
		ImGuiIO& io = ImGui::GetIO();
		auto boldFont = io.Fonts->Fonts[0];

		ImGui::PushID(label.c_str());
		ImGui::PushItemWidth(-1);

		ImGui::Columns(2);
		ImGui::AlignTextToFramePadding();
		ImGui::Text(label.c_str());
		ImGui::NextColumn();

		ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, 5 });

		float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
		ImVec2 buttonSize = { lineHeight + 3.0f, lineHeight };

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.9f, 0.2f, 0.2f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
		ImGui::PushFont(boldFont);
		if (ImGui::Button("X", buttonSize))
			values.x = resetValue;
		ImGui::PopFont();
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		ImGui::DragFloat("##X", &values.x, 0.1f, 0.0f, 0.0f, "%.2f");
		ImGui::PopItemWidth();
		ImGui::SameLine();

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.3f, 0.8f, 0.3f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
		ImGui::PushFont(boldFont);
		if (ImGui::Button("Y", buttonSize))
			values.y = resetValue;
		ImGui::PopFont();
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		ImGui::DragFloat("##Y", &values.y, 0.1f, 0.0f, 0.0f, "%.2f");
		ImGui::PopItemWidth();
		ImGui::SameLine();

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.2f, 0.35f, 0.9f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
		ImGui::PushFont(boldFont);
		if (ImGui::Button("Z", buttonSize))
			values.z = resetValue;
		ImGui::PopFont();
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		ImGui::DragFloat("##Z", &values.z, 0.1f, 0.0f, 0.0f, "%.2f");
		ImGui::PopItemWidth();

		ImGui::PopStyleVar();

		ImGui::Columns(1);

		ImGui::PopItemWidth();
		ImGui::PopID();
	}

	void DrawColorControl(const std::string& label, glm::vec3& values, float resetValue, float columnWidth)
	{
		ImGuiIO& io = ImGui::GetIO();
		auto boldFont = io.Fonts->Fonts[0];
		int i[3] = { IM_F32_TO_INT8_UNBOUND(values.x), IM_F32_TO_INT8_UNBOUND(values.y), IM_F32_TO_INT8_UNBOUND(values.z)};

		ImGui::PushID(label.c_str());
		ImGui::PushItemWidth(-1);

		ImGui::Columns(2);
		ImGui::AlignTextToFramePadding();
		ImGui::Text(label.c_str());
		ImGui::NextColumn();

		ImGui::PushMultiItemsWidths(4, ImGui::CalcItemWidth());
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, 5 });

		float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
		ImVec2 buttonSize = { lineHeight + 3.0f, lineHeight };

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.9f, 0.2f, 0.2f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
		ImGui::PushFont(boldFont);
		if (ImGui::Button("R", buttonSize))
			values.x = 0;
		ImGui::PopFont();
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		ImGui::DragInt("##R", &i[0], 1.0f, 0.0f, 255, "%3d");
		values.x = i[0] / 255.0f;
		ImGui::PopItemWidth();
		ImGui::SameLine();

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.3f, 0.8f, 0.3f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
		ImGui::PushFont(boldFont);
		if (ImGui::Button("G", buttonSize))
			values.y = 0;
		ImGui::PopFont();
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		ImGui::DragInt("##G", &i[1], 1.0f, 0.0f, 255, "%3d");
		values.y = i[1] / 255.0f;
		ImGui::PopItemWidth();
		ImGui::SameLine();

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.2f, 0.35f, 0.9f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
		ImGui::PushFont(boldFont);
		if (ImGui::Button("B", buttonSize))
			values.z = 0;
		ImGui::PopFont();
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		ImGui::DragInt("##B", &i[2], 1.0f, 0.0f, 255, "%3d");
		values.z = i[2] / 255.0f;
		ImGui::PopItemWidth();
		ImGui::SameLine();

		if (ImGui::ColorButton("##ColorButton", { values.x, values.y, values.z, 1.0f }))
			ImGui::OpenPopup("ColorPicker");

		ImGui::PopItemWidth();

		if (ImGui::BeginPopup("ColorPicker"))
		{

			ImGuiColorEditFlags picker_flags_to_forward = ImGuiColorEditFlags_DataTypeMask_ | ImGuiColorEditFlags_PickerMask_ | ImGuiColorEditFlags_InputMask_ | ImGuiColorEditFlags_HDR | ImGuiColorEditFlags_NoAlpha | ImGuiColorEditFlags_AlphaBar;
			ImGuiColorEditFlags picker_flags = ImGuiColorEditFlags_DisplayMask_ | ImGuiColorEditFlags_NoLabel | ImGuiColorEditFlags_AlphaPreviewHalf;
			ImGui::ColorPicker3("##ColorPicker", (float*)&values, picker_flags);
			ImGui::EndPopup();
		}

		ImGui::PopStyleVar();
		ImGui::Columns(1);

		ImGui::PopItemWidth();
		ImGui::PopID();
	}

	void DrawFloatControl(const std::string& label, float& value, float columnWidth)
	{
		ImGui::PushID(label.c_str());

		ImGui::PushItemWidth(-1);
		ImGui::Columns(2);
		ImGui::AlignTextToFramePadding();
		ImGui::Text(label.c_str());
		ImGui::NextColumn();

		ImGui::DragFloat("##X", &value, 0.1f, 0.0f, 0.0f, "%.2f");

		ImGui::Columns(1);

		ImGui::PopItemWidth();
		ImGui::PopID();
	}

	void DrawTextureControl(const std::string& label, Texture*& texture, float size)
	{
		ImGui::PushID(label.c_str());

		ImGui::AlignTextToFramePadding();
		ImGui::Text(label.c_str());

		ImGui::Columns(2);
		ImGui::Image((ImTextureID)texture->GetRenderId(), ImVec2{ size, size }, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });
		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_TEXTURE"))
				texture = *(Texture**)payload->Data;

			ImGui::EndDragDropTarget();
		}
		ImGui::NextColumn();

		std::filesystem::path path = std::string(texture->path);
		ImGui::Text(path.filename().string().c_str());

		if (ImGui::BeginPopupContextItem("test"))
		{
			if (ImGui::MenuItem("Save Texture"))
				(texture)->Save("Test.texture");

			if (ImGui::MenuItem("Load Texture"))
				(texture)->LoadAsync("Test.texture");

			ImGui::EndPopup();
		}

		ImGui::Columns(1);
		ImGui::PopID();
	}

	void DrawCubemapControl(const std::string& label, HdrCubemap** cubemap, float size)
	{
		ResourceManager& resourceManager = ((Editor*)Application::Get())->editorResourceManager;
		ImGui::PushID(label.c_str());

		ImGui::AlignTextToFramePadding();
		ImGui::Text(label.c_str());

		ImGui::Columns(2);
		ImGui::Image((ImTextureID)resourceManager.GetTexture("Resources/FileIcon.png")->GetRenderId(), ImVec2{ size, size }, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });

		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_CUBEMAP"))
				*cubemap = *(HdrCubemap**)payload->Data;

			ImGui::EndDragDropTarget();
		}
		ImGui::NextColumn();

		std::filesystem::path path = std::string((*cubemap)->GetPath());
		ImGui::Text(path.filename().string().c_str());

		if (ImGui::BeginPopupContextItem("test"))
		{
			if (ImGui::MenuItem("Save Cubemap"))
				(*cubemap)->Save("Test.cubemap");

			if (ImGui::MenuItem("Load Texture"))
				(*cubemap)->Load("Test.cubemap");

			ImGui::EndPopup();
		}
		
		ImGui::Columns(1);
		ImGui::PopID();
	}

	void DrawMeshControl(const std::string& label, Mesh** mesh, float size)
	{
		ResourceManager& resourceManager = ((Editor*)Application::Get())->editorResourceManager;
		ImGui::PushID(label.c_str());

		ImGui::AlignTextToFramePadding();
		ImGui::Text(label.c_str());

		ImGui::Columns(2);
		ImGui::Image((ImTextureID)resourceManager.GetTexture("Resources/ModelIcon.png")->GetRenderId(), ImVec2{ size, size }, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });

		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_MESH"))
				*mesh = *(Mesh**)payload->Data;

			ImGui::EndDragDropTarget();
		}

		ImGui::NextColumn();

		ImGui::Text((*mesh)->name.c_str());
		
		ImGui::Columns(1);
		ImGui::PopID();
	}

	bool DrawMaterialControl(const std::string& label, Material** material, float size)
	{
		ResourceManager& resourceManager = ((Editor*)Application::Get())->editorResourceManager;
		bool clicked = false;
		static Material* selectedMaterial = nullptr;

		ImGui::PushID(label.c_str());

		ImGui::AlignTextToFramePadding();
		ImGui::Text(label.c_str());

		ImGui::Columns(2);
		ImGui::Image((ImTextureID)resourceManager.GetTexture("Resources/MaterialIcon.png")->GetRenderId(), ImVec2{ size, size }, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });
		
		if (ImGui::IsItemClicked())
			clicked = true;

		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_MATERIAL"))
				*material = *(Material**)payload->Data;

			ImGui::EndDragDropTarget();
		}

		ImGui::NextColumn();

		ImGui::Text((*material)->name.c_str());

		ImGui::Columns(1);

		ImGui::PopID();
		
		return clicked;
	}

	void DrawMaterialEditor(const std::string& label, Material* material, bool* open)
	{
		if (ImGui::Begin(label.c_str(), open))
		{
			DrawColorControl("Tint", material->tint);

			DrawTextureControl("Albedo", material->albedo);
			DrawTextureControl("Normal", material->normal);
			DrawTextureControl("Roughness", material->roughness);
			DrawTextureControl("Metallic", material->metallic);
			DrawTextureControl("Ambient Occlusion", material->ao);
		}
		
		ImGui::End();
	}

	void DrawArmatureControl(const std::string& label, Armature** armature, float size)
	{
		ResourceManager& resourceManager = ((Editor*)Application::Get())->editorResourceManager;
		ImGui::PushID(label.c_str());

		ImGui::AlignTextToFramePadding();
		ImGui::Text(label.c_str());

		ImGui::Columns(2);
		ImGui::Image((ImTextureID)resourceManager.GetOrLoadTexture("Resources/ArmatureIcon.png")->GetRenderId(), ImVec2{ size, size }, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });

		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ARMATURE"))
				*armature = *(Armature**)payload->Data;

			ImGui::EndDragDropTarget();
		}

		ImGui::NextColumn();

		ImGui::Text((*armature)->name.c_str());

		ImGui::Columns(1);
		ImGui::PopID();
	}

	void DrawAnimationControl(const std::string& label, Animation** animation, float size)
	{
		ResourceManager& resourceManager = ((Editor*)Application::Get())->editorResourceManager;
		ImGui::PushID(label.c_str());

		ImGui::AlignTextToFramePadding();
		ImGui::Text(label.c_str());

		ImGui::Columns(2);
		ImGui::Image((ImTextureID)resourceManager.GetOrLoadTexture("Resources/AnimationIcon.png")->GetRenderId(), ImVec2{ size, size }, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });

		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ANIMATION"))
				*animation = *(Animation**)payload->Data;

			ImGui::EndDragDropTarget();
		}

		ImGui::NextColumn();

		ImGui::Text((*animation)->name.c_str());

		ImGui::Columns(1);
		ImGui::PopID();
	}

	void DrawReflectedMember(void* object, MemberData& member)
	{
		char* obj = (char*) object;
		if (member.type == Types::FLOAT)
			DrawFloatControl(member.name.c_str(), *(float*)(obj + member.offset));

		if (member.type == Types::VECTOR3)
			DrawVec3Control(member.name.c_str(), *(glm::vec3*)(obj + member.offset), 0.0f);

		if (member.type == Types::VECTOR3_ANGLES)
		{
			glm::vec3 temp = glm::degrees(*(glm::vec3*)(obj + member.offset));
			DrawVec3Control(member.name.c_str(), temp, 0.0f);
			*(glm::vec3*)(obj + member.offset) = glm::radians(temp);
		}

		if (member.type == Types::VECTOR3_COLOR)
			DrawColorControl(member.name.c_str(), *(glm::vec3*)(obj + member.offset));

		if (member.type == Types::TEXTURE)
		{
			Texture* t = *(Texture**)(obj + member.offset);
			DrawTextureControl(member.name.c_str(), t);
		}

		if (member.type == Types::MESH)
		{
			Mesh** m = (Mesh**)(obj + member.offset);
			DrawMeshControl(member.name.c_str(), m);
		}

		if (member.type == Types::MATERIAL_VECTOR)
		{
			std::vector<Material*>* v = (std::vector<Material*>*)(obj + member.offset);

			static bool open;
			static int selected;

			for (int i = 0; i < v->size(); i++)
				if (DrawMaterialControl(member.name.c_str(), &v->at(i)))
				{
					if (selected == i && open)
						open = false;
					else
					{
						selected = i;
						open = true;
					}
				}

			if (open && selected < v->size())
				DrawMaterialEditor("Material Editor", v->at(selected), &open);
		}

		if (member.type == Types::CUBEMAP)
		{
			HdrCubemap** c = (HdrCubemap**)(obj + member.offset);
			DrawCubemapControl(member.name.c_str(), c);
		}

		if (member.type == Types::ARMATURE)
		{
			Armature** a = (Armature**)(obj + member.offset);
			DrawArmatureControl(member.name.c_str(), a);
		}

		if (member.type == Types::ANIMATION)
		{
			Animation** a = (Animation**)(obj + member.offset);
			DrawAnimationControl(member.name.c_str(), a);
		}
	}

	void DrawReflectedMembers(void* object, std::vector<MemberData>& members)
	{
		for (MemberData& member : members)
			DrawReflectedMember(object, member);
	}
}