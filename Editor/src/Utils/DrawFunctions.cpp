#include "DrawFunctions.h"

#include "../Editor.h"

#include <Imgui/imgui_stdlib.h>

namespace Seidon
{
	bool DrawVec3Control(const std::string& label, glm::vec3& values, float resetValue, float columnWidth)
	{
		ImGuiIO& io = ImGui::GetIO();
		auto boldFont = io.Fonts->Fonts[0];

		bool changed = false;

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
		{
			values.x = resetValue;
			changed = true;
		}

		ImGui::PopFont();
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		changed |= ImGui::DragFloat("##X", &values.x, 0.1f, 0.0f, 0.0f, "%.2f");
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
		}
		ImGui::PopFont();
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		changed |= ImGui::DragFloat("##Y", &values.y, 0.1f, 0.0f, 0.0f, "%.2f");
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
		}
		ImGui::PopFont();
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		changed |= ImGui::DragFloat("##Z", &values.z, 0.1f, 0.0f, 0.0f, "%.2f");
		ImGui::PopItemWidth();

		ImGui::PopStyleVar();

		ImGui::Columns(1);

		ImGui::PopItemWidth();
		ImGui::PopID();

		return changed;
	}

	bool DrawColorControl(const std::string& label, glm::vec3& values, float resetValue, float columnWidth)
	{
		ImGuiIO& io = ImGui::GetIO();
		auto boldFont = io.Fonts->Fonts[0];
		int i[3] = { IM_F32_TO_INT8_UNBOUND(values.x), IM_F32_TO_INT8_UNBOUND(values.y), IM_F32_TO_INT8_UNBOUND(values.z)};

		bool changed = false;

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
		{
			values.x = 0;
			changed = true;
		}
		ImGui::PopFont();
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		changed |= ImGui::DragInt("##R", &i[0], 1.0f, 0.0f, 255, "%3d");
		values.x = i[0] / 255.0f;
		ImGui::PopItemWidth();
		ImGui::SameLine();

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.3f, 0.8f, 0.3f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
		ImGui::PushFont(boldFont);
		if (ImGui::Button("G", buttonSize))
		{
			values.y = 0;
			changed = true;
		}
		ImGui::PopFont();
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		changed |= ImGui::DragInt("##G", &i[1], 1.0f, 0.0f, 255, "%3d");
		values.y = i[1] / 255.0f;
		ImGui::PopItemWidth();
		ImGui::SameLine();

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.2f, 0.35f, 0.9f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
		ImGui::PushFont(boldFont);
		if (ImGui::Button("B", buttonSize))
		{
			values.z = 0;
			changed = true;
		}
		ImGui::PopFont();
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		changed |= ImGui::DragInt("##B", &i[2], 1.0f, 0.0f, 255, "%3d");
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
			changed |= ImGui::ColorPicker3("##ColorPicker", (float*)&values, picker_flags);
			ImGui::EndPopup();
		}

		ImGui::PopStyleVar();
		ImGui::Columns(1);

		ImGui::PopItemWidth();
		ImGui::PopID();

		return changed;
	}

	bool DrawFloatControl(const std::string& label, float& value, float columnWidth)
	{
		ImGui::PushID(label.c_str());

		ImGui::PushItemWidth(-1);
		ImGui::Columns(2);
		ImGui::AlignTextToFramePadding();
		ImGui::Text(label.c_str());
		ImGui::NextColumn();

		bool changed = ImGui::DragFloat("##X", &value, 0.1f, 0.0f, 0.0f, "%.2f");

		ImGui::Columns(1);

		ImGui::PopItemWidth();
		ImGui::PopID();

		return changed;
	}

	bool DrawFloatSliderControl(const std::string& label, float& value, float min, float max, float columnWidth)
	{
		ImGui::PushID(label.c_str());

		ImGui::PushItemWidth(-1);
		ImGui::Columns(2);
		ImGui::AlignTextToFramePadding();
		ImGui::Text(label.c_str());
		ImGui::NextColumn();

		bool changed = ImGui::SliderFloat("##X", &value, min, max, "%.2f");

		ImGui::Columns(1);

		ImGui::PopItemWidth();
		ImGui::PopID();

		return changed;
	}

	bool DrawBoolControl(const std::string& label, bool& value, float columnWidth)
	{
		ImGui::PushID(label.c_str());

		ImGui::PushItemWidth(-1);
		ImGui::Columns(2);
		ImGui::AlignTextToFramePadding();
		ImGui::Text(label.c_str());
		ImGui::NextColumn();

		bool changed = ImGui::Checkbox("##bool", &value);

		ImGui::Columns(1);

		ImGui::PopItemWidth();
		ImGui::PopID();

		return changed;
	}

	bool DrawStringControl(const std::string& label, std::string& value, float columnWidth)
	{
		ImGui::PushID(label.c_str());

		ImGui::PushItemWidth(-1);
		ImGui::Columns(2);
		ImGui::AlignTextToFramePadding();
		ImGui::Text(label.c_str());
		ImGui::NextColumn();

		bool changed = ImGui::InputText("##string", &value);

		ImGui::Columns(1);

		ImGui::PopItemWidth();
		ImGui::PopID();

		return changed;
	}

	bool DrawTextureControl(const std::string& label, Texture** texture, float size)
	{
		bool changed = false;
		ImGui::PushID(label.c_str());

		ImGui::AlignTextToFramePadding();
		ImGui::Text(label.c_str());

		ImGui::Columns(2);
		ImGui::Image((ImTextureID)(*texture)->GetRenderId(), ImVec2{ size, size }, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });
		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("FILE_BROWSER_TEXTURE"))
			{
				ResourceManager& resourceManager = *Application::Get()->GetResourceManager();
				std::string path = (const char*)payload->Data;
				
				*texture = resourceManager.GetOrLoadTexture(path);

				changed = true;
			}

			ImGui::EndDragDropTarget();
		}
		ImGui::NextColumn();

		std::filesystem::path path = std::string((*texture)->path);
		ImGui::Text(path.filename().string().c_str());

		ImGui::Columns(1);
		ImGui::PopID();

		return changed;
	}

	bool DrawCubemapControl(const std::string& label, HdrCubemap** cubemap, float size)
	{
		ResourceManager& resourceManager = ((Editor*)Application::Get())->editorResourceManager;
		bool changed = false;

		ImGui::PushID(label.c_str());

		ImGui::AlignTextToFramePadding();
		ImGui::Text(label.c_str());

		ImGui::Columns(2);
		ImGui::Image((ImTextureID)resourceManager.GetOrLoadTexture("Resources/FileIcon.sdtex")->GetRenderId(), ImVec2{ size, size }, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });

		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("FILE_BROWSER_CUBEMAP"))
			{
				ResourceManager& resourceManager = *Application::Get()->GetResourceManager();
				std::string path = (const char*)payload->Data;

				*cubemap = resourceManager.GetOrLoadCubemap(path);

				changed = true;
			}

			ImGui::EndDragDropTarget();
		}
		ImGui::NextColumn();

		std::filesystem::path path = std::string((*cubemap)->GetPath());
		ImGui::Text(path.filename().string().c_str());
		
		ImGui::Columns(1);
		ImGui::PopID();

		return changed;
	}

	bool DrawMeshControl(const std::string& label, Mesh** mesh, float size)
	{
		ResourceManager& resourceManager = ((Editor*)Application::Get())->editorResourceManager;
		bool changed = false;

		ImGui::PushID(label.c_str());

		ImGui::AlignTextToFramePadding();
		ImGui::Text(label.c_str());

		ImGui::Columns(2);
		ImGui::Image((ImTextureID)resourceManager.GetOrLoadTexture("Resources/ModelIcon.sdtex")->GetRenderId(), ImVec2{ size, size }, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });

		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("FILE_BROWSER_MESH"))
			{
				ResourceManager& resourceManager = *Application::Get()->GetResourceManager();
				std::string path = (const char*)payload->Data;

				*mesh = resourceManager.GetOrLoadMesh(path);

				changed = true;
			}

			ImGui::EndDragDropTarget();
		}

		ImGui::NextColumn();

		ImGui::Text((*mesh)->name.c_str());
		
		ImGui::Columns(1);
		ImGui::PopID();

		return changed;
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
		ImGui::Image((ImTextureID)resourceManager.GetOrLoadTexture("Resources/MaterialIcon.sdtex")->GetRenderId(), ImVec2{ size, size }, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });
		
		if (ImGui::IsItemClicked())
			clicked = true;

		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("FILE_BROWSER_MATERIAL"))
			{
				ResourceManager& resourceManager = *Application::Get()->GetResourceManager();
				std::string path = (const char*)payload->Data;

				*material = resourceManager.GetOrLoadMaterial(path);
			}

			ImGui::EndDragDropTarget();
		}

		ImGui::NextColumn();

		ImGui::Text((*material)->name.c_str());

		ImGui::Columns(1);

		ImGui::PopID();
		
		return clicked;
	}

	bool DrawMaterialEditor(const std::string& label, Material* material, bool* open)
	{
		ResourceManager& resourceManager = *Application::Get()->GetResourceManager();
		bool changed = false;

		if (ImGui::Begin(label.c_str(), open))
		{
			DrawStringControl("Name", material->name);
			
			if (DrawShaderControl("Shader", &material->shader))
			{
				memset(material->data, 0, 500);

				for (MemberData& m : material->shader->GetBufferLayout()->members)
					switch (m.type)
					{
					case Types::VECTOR3_COLOR: 
						*(glm::vec3*)(material->data + m.offset) = glm::vec3(1);
						break;

					case Types::TEXTURE:
						*(Texture**)(material->data + m.offset) = resourceManager.GetTexture("albedo_default");
						break;
					}

				changed = true;
			}

			changed = DrawMetaType(material->data, *material->shader->GetBufferLayout());

			ImGuiStyle& style = ImGui::GetStyle();
			float size = ImGui::CalcTextSize("Save").x + style.FramePadding.x * 2.0f;

			float offset = (ImGui::GetContentRegionAvail().x - size) * 0.5;
			ImGui::SetCursorPosX(ImGui::GetCursorPosX() + offset);
			if (ImGui::Button("Save"))
				material->Save(Application::Get()->GetResourceManager()->GetMaterialPath(material->id));
		
			}
			ImGui::End();

			return changed;
	}

	bool DrawArmatureControl(const std::string& label, Armature** armature, float size)
	{
		ResourceManager& resourceManager = ((Editor*)Application::Get())->editorResourceManager;
		bool changed = false;

		ImGui::PushID(label.c_str());

		ImGui::AlignTextToFramePadding();
		ImGui::Text(label.c_str());

		ImGui::Columns(2);
		ImGui::Image((ImTextureID)resourceManager.GetOrLoadTexture("Resources/ArmatureIcon.sdtex")->GetRenderId(), ImVec2{ size, size }, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });

		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("FILE_BROWSER_ARMATURE"))
			{
				ResourceManager& resourceManager = *Application::Get()->GetResourceManager();
				std::string path = (const char*)payload->Data;

				*armature = resourceManager.GetOrLoadArmature(path);

				changed = true;
			}

			ImGui::EndDragDropTarget();
		}

		ImGui::NextColumn();

		ImGui::Text((*armature)->name.c_str());

		ImGui::Columns(1);
		ImGui::PopID();

		return changed;
	}

	bool DrawAnimationControl(const std::string& label, Animation** animation, float size)
	{
		ResourceManager& resourceManager = ((Editor*)Application::Get())->editorResourceManager;
		bool changed = false;
		ImGui::PushID(label.c_str());

		ImGui::AlignTextToFramePadding();
		ImGui::Text(label.c_str());

		ImGui::Columns(2);
		ImGui::Image((ImTextureID)resourceManager.GetOrLoadTexture("Resources/AnimationIcon.sdtex")->GetRenderId(), ImVec2{ size, size }, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });

		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("FILE_BROWSER_ANIMATION"))
			{
				ResourceManager& resourceManager = *Application::Get()->GetResourceManager();
				std::string path = (const char*)payload->Data;

				*animation = resourceManager.GetOrLoadAnimation(path);

				changed = true;
			}

			ImGui::EndDragDropTarget();
		}

		ImGui::NextColumn();

		ImGui::Text((*animation)->name.c_str());

		ImGui::Columns(1);
		ImGui::PopID();

		return changed;
	}

	bool DrawShaderControl(const std::string& label, Shader** shader, float size)
	{
		ResourceManager& resourceManager = ((Editor*)Application::Get())->editorResourceManager;
		bool changed = false;

		ImGui::PushID(label.c_str());

		ImGui::AlignTextToFramePadding();
		ImGui::Text(label.c_str());

		ImGui::Columns(2);
		ImGui::Image((ImTextureID)resourceManager.GetOrLoadTexture("Resources/FileIcon.sdtex")->GetRenderId(), ImVec2{ size, size }, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });

		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("FILE_BROWSER_SHADER"))
			{
				ResourceManager& resourceManager = *Application::Get()->GetResourceManager();
				std::string path = (const char*)payload->Data;

				*shader = resourceManager.GetOrLoadShader(path);
				changed = true;
			}

			ImGui::EndDragDropTarget();
		}

		ImGui::NextColumn();

		ImGui::Text((*shader)->GetPath().c_str());

		ImGui::Columns(1);
		ImGui::PopID();

		return changed;
	}

	bool DrawReflectedMember(void* object, MemberData& member)
	{
		bool changed = false;

		char* obj = (char*) object;
		if (member.type == Types::FLOAT)
			changed = DrawFloatControl(member.name.c_str(), *(float*)(obj + member.offset));

		if (member.type == Types::FLOAT_NORMALIZED)
			changed = DrawFloatSliderControl(member.name.c_str(), *(float*)(obj + member.offset));

		if (member.type == Types::BOOL)
			changed = DrawBoolControl(member.name.c_str(), *(bool*)(obj + member.offset));

		if (member.type == Types::STRING)
			changed = DrawStringControl(member.name.c_str(), *(std::string*)(obj + member.offset));

		if (member.type == Types::VECTOR3)
			changed = DrawVec3Control(member.name.c_str(), *(glm::vec3*)(obj + member.offset), 0.0f);

		if (member.type == Types::VECTOR3_ANGLES)
		{
			glm::vec3 temp = glm::degrees(*(glm::vec3*)(obj + member.offset));
			changed = DrawVec3Control(member.name.c_str(), temp, 0.0f);
			*(glm::vec3*)(obj + member.offset) = glm::radians(temp);
		}

		if (member.type == Types::VECTOR3_COLOR)
			changed = DrawColorControl(member.name.c_str(), *(glm::vec3*)(obj + member.offset));

		if (member.type == Types::TEXTURE)
		{
			Texture** t = (Texture**)(obj + member.offset);
			changed = DrawTextureControl(member.name.c_str(), t);
		}

		if (member.type == Types::MESH)
		{
			Mesh** m = (Mesh**)(obj + member.offset);
			changed = DrawMeshControl(member.name.c_str(), m);
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
				changed = DrawMaterialEditor("Material Editor", v->at(selected), &open);
		}

		if (member.type == Types::CUBEMAP)
		{
			HdrCubemap** c = (HdrCubemap**)(obj + member.offset);
			changed = DrawCubemapControl(member.name.c_str(), c);
		}

		if (member.type == Types::ARMATURE)
		{
			Armature** a = (Armature**)(obj + member.offset);
			changed = DrawArmatureControl(member.name.c_str(), a);
		}

		if (member.type == Types::ANIMATION)
		{
			Animation** a = (Animation**)(obj + member.offset);
			changed = DrawAnimationControl(member.name.c_str(), a);
		}

		return changed;
	}

	bool DrawMetaType(void* object, MetaType& type)
	{
		bool changed = false;

		for (MemberData& member : type.members)
			changed |= DrawReflectedMember(object, member);

		if (changed && type.OnChange) type.OnChange(object);

		return changed;
	}
}