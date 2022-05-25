#include "DrawFunctions.h"

#include "../Editor.h"

#include <Imgui/imgui_stdlib.h>

namespace Seidon
{
	ChangeStatus DrawVec3Control(const std::string& label, glm::vec3& values, float resetValue, float columnWidth)
	{
		ImGuiIO& io = ImGui::GetIO();
		auto boldFont = io.Fonts->Fonts[0];

		bool changed = false;
		bool stoppedChanging = false;

		glm::vec3 oldValue = values;

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
			stoppedChanging = true;
		}

		ImGui::PopFont();
		ImGui::PopStyleColor(3);

		ImGui::SameLine();

		changed |= ImGui::DragFloat("##X", &values.x, 0.1f, 0.0f, 0.0f, "%.2f");
		if (ImGui::IsItemDeactivatedAfterEdit()) stoppedChanging = true;

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
		}
		ImGui::PopFont();
		ImGui::PopStyleColor(3);

		ImGui::SameLine();

		changed |= ImGui::DragFloat("##Y", &values.y, 0.1f, 0.0f, 0.0f, "%.2f");
		if (ImGui::IsItemDeactivatedAfterEdit()) stoppedChanging = true;

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
		}
		ImGui::PopFont();
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		
		changed |= ImGui::DragFloat("##Z", &values.z, 0.1f, 0.0f, 0.0f, "%.2f");
		if (ImGui::IsItemDeactivatedAfterEdit()) stoppedChanging = true;

		ImGui::PopItemWidth();

		ImGui::PopStyleVar();

		ImGui::Columns(1);

		ImGui::PopItemWidth();
		ImGui::PopID();

		if (stoppedChanging) return ChangeStatus::CHANGED;
		if (changed) return ChangeStatus::CHANGING;

		return ChangeStatus::NO_CHANGE;
	}

	ChangeStatus DrawColorControl(const std::string& label, glm::vec3& values, float resetValue, float columnWidth)
	{
		ImGuiIO& io = ImGui::GetIO();
		auto boldFont = io.Fonts->Fonts[0];
		int i[3] = { IM_F32_TO_INT8_UNBOUND(values.x), IM_F32_TO_INT8_UNBOUND(values.y), IM_F32_TO_INT8_UNBOUND(values.z)};

		bool changed = false;
		bool stoppedChanging = false;

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
			stoppedChanging = true;
		}
		ImGui::PopFont();
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		changed |= ImGui::DragInt("##R", &i[0], 1.0f, 0.0f, 255, "%3d");
		values.x = i[0] / 255.0f;

		if (ImGui::IsItemDeactivatedAfterEdit()) stoppedChanging = true;

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
			stoppedChanging = true;
		}
		ImGui::PopFont();
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		changed |= ImGui::DragInt("##G", &i[1], 1.0f, 0.0f, 255, "%3d");
		values.y = i[1] / 255.0f;

		if (ImGui::IsItemDeactivatedAfterEdit()) stoppedChanging = true;

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
			stoppedChanging = true;
		}
		ImGui::PopFont();
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		changed |= ImGui::DragInt("##B", &i[2], 1.0f, 0.0f, 255, "%3d");
		values.z = i[2] / 255.0f;

		if (ImGui::IsItemDeactivatedAfterEdit()) stoppedChanging = true;

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

		if (stoppedChanging) return ChangeStatus::CHANGED;
		if (changed) return ChangeStatus::CHANGING;

		return ChangeStatus::NO_CHANGE;
	}

	ChangeStatus DrawIdControl(const std::string& label, UUID& value, float columnWidth)
	{
		ImGui::PushID(label.c_str());

		bool changed = false;

		ImGui::PushItemWidth(-1);
		ImGui::Columns(2);
		ImGui::AlignTextToFramePadding();
		ImGui::Text(label.c_str());
		ImGui::NextColumn();

		std::string s = std::to_string(value);
		ImGui::InputText("##X", &s, ImGuiInputTextFlags_ReadOnly);

		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("HIERARCHY_PANEL_ENTITY"))
			{
				Entity& entity = *(Entity*)payload->Data;

				value = entity.GetId();

				changed = true;
			}

			ImGui::EndDragDropTarget();
		}

		ImGui::Columns(1);

		ImGui::PopItemWidth();
		ImGui::PopID();

		if (changed) return ChangeStatus::CHANGED;

		return ChangeStatus::NO_CHANGE;
	}

	ChangeStatus DrawFloatControl(const std::string& label, float& value, float columnWidth)
	{
		ImGui::PushID(label.c_str());

		ImGui::PushItemWidth(-1);
		ImGui::Columns(2);
		ImGui::AlignTextToFramePadding();
		ImGui::Text(label.c_str());
		ImGui::NextColumn();

		bool changed = ImGui::DragFloat("##X", &value, 0.1f, 0.0f, 0.0f, "%.2f");
		bool stoppedChanging = false;

		if (ImGui::IsItemDeactivatedAfterEdit()) stoppedChanging = true;

		ImGui::Columns(1);

		ImGui::PopItemWidth();
		ImGui::PopID();

		if (stoppedChanging) return ChangeStatus::CHANGED;
		if (changed) return ChangeStatus::CHANGING;

		return ChangeStatus::NO_CHANGE;
	}

	ChangeStatus DrawFloatSliderControl(const std::string& label, float& value, float min, float max, float columnWidth)
	{
		ImGui::PushID(label.c_str());

		ImGui::PushItemWidth(-1);
		ImGui::Columns(2);
		ImGui::AlignTextToFramePadding();
		ImGui::Text(label.c_str());
		ImGui::NextColumn();

		bool changed = ImGui::SliderFloat("##X", &value, min, max, "%.2f");

		bool stoppedChanging = false;
		if (ImGui::IsItemDeactivatedAfterEdit()) stoppedChanging = true;

		ImGui::Columns(1);

		ImGui::PopItemWidth();
		ImGui::PopID();

		if (stoppedChanging) return ChangeStatus::CHANGED;
		if (changed) return ChangeStatus::CHANGING;

		return ChangeStatus::NO_CHANGE;
	}

	ChangeStatus DrawBoolControl(const std::string& label, bool& value, float columnWidth)
	{
		ImGui::PushID(label.c_str());

		ImGui::PushItemWidth(-1);
		ImGui::Columns(2);
		ImGui::AlignTextToFramePadding();
		ImGui::Text(label.c_str());
		ImGui::NextColumn();

		bool changed = ImGui::Checkbox("##bool", &value);

		bool stoppedChanging = false;
		if (ImGui::IsItemDeactivatedAfterEdit()) stoppedChanging = true;

		ImGui::Columns(1);

		ImGui::PopItemWidth();
		ImGui::PopID();

		if (stoppedChanging) return ChangeStatus::CHANGED;
		if (changed) return ChangeStatus::CHANGING;

		return ChangeStatus::NO_CHANGE;
	}

	ChangeStatus DrawStringControl(const std::string& label, std::string& value, float columnWidth)
	{
		ImGui::PushID(label.c_str());

		ImGui::PushItemWidth(-1);
		ImGui::Columns(2);
		ImGui::AlignTextToFramePadding();
		ImGui::Text(label.c_str());
		ImGui::NextColumn();

		bool changed = ImGui::InputText("##string", &value);

		bool stoppedChanging = false;
		if (ImGui::IsItemDeactivatedAfterEdit()) stoppedChanging = true;

		ImGui::Columns(1);

		ImGui::PopItemWidth();
		ImGui::PopID();

		if (stoppedChanging) return ChangeStatus::CHANGED;
		if (changed) return ChangeStatus::CHANGING;

		return ChangeStatus::NO_CHANGE;
	}

	ChangeStatus DrawTextureControl(const std::string& label, Texture** texture, float size)
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

		if (changed) return ChangeStatus::CHANGED;

		return ChangeStatus::NO_CHANGE;
	}

	ChangeStatus DrawCubemapControl(const std::string& label, HdrCubemap** cubemap, float size)
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

		if (changed) return ChangeStatus::CHANGED;

		return ChangeStatus::NO_CHANGE;
	}

	ChangeStatus DrawMeshControl(const std::string& label, Mesh** mesh, float size)
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

		if (changed) return ChangeStatus::CHANGED;

		return ChangeStatus::NO_CHANGE;
	}

	ChangeStatus DrawSkinnedMeshControl(const std::string& label, SkinnedMesh** mesh, float size)
	{
		ResourceManager& resourceManager = ((Editor*)Application::Get())->editorResourceManager;
		bool changed = false;

		ImGui::PushID(label.c_str());

		ImGui::AlignTextToFramePadding();
		ImGui::Text(label.c_str());

		ImGui::Columns(2);
		ImGui::Image((ImTextureID)resourceManager.GetOrLoadTexture("Resources/SkinnedMeshIcon.sdtex")->GetRenderId(), ImVec2{ size, size }, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });

		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("FILE_BROWSER_SKINNED_MESH"))
			{
				ResourceManager& resourceManager = *Application::Get()->GetResourceManager();
				std::string path = (const char*)payload->Data;

				*mesh = resourceManager.GetOrLoadSkinnedMesh(path);

				changed = true;
			}

			ImGui::EndDragDropTarget();
		}

		ImGui::NextColumn();

		ImGui::Text((*mesh)->name.c_str());

		ImGui::Columns(1);
		ImGui::PopID();

		if (changed) return ChangeStatus::CHANGED;

		return ChangeStatus::NO_CHANGE;
	}

	ChangeStatus DrawMaterialControl(const std::string& label, Material** material, float size, bool* clicked)
	{
		ResourceManager& resourceManager = ((Editor*)Application::Get())->editorResourceManager;
		static Material* selectedMaterial = nullptr;

		bool changed = false;

		ImGui::PushID(label.c_str());

		ImGui::AlignTextToFramePadding();
		ImGui::Text(label.c_str());

		ImGui::Columns(2);
		ImGui::Image((ImTextureID)resourceManager.GetOrLoadTexture("Resources/MaterialIcon.sdtex")->GetRenderId(), ImVec2{ size, size }, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });
		
		if (ImGui::IsItemClicked() && clicked)
			*clicked = true;

		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("FILE_BROWSER_MATERIAL"))
			{
				ResourceManager& resourceManager = *Application::Get()->GetResourceManager();
				std::string path = (const char*)payload->Data;

				*material = resourceManager.GetOrLoadMaterial(path);

				changed = true;
			}

			ImGui::EndDragDropTarget();
		}

		ImGui::NextColumn();

		ImGui::Text((*material)->name.c_str());

		ImGui::Columns(1);

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
					case Types::VECTOR3_COLOR: 
						*(glm::vec3*)(material->data + m.offset) = glm::vec3(1);
						break;

					case Types::TEXTURE:
						*(Texture**)(material->data + m.offset) = resourceManager.GetTexture("albedo_default");
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
				material->Save(Application::Get()->GetResourceManager()->GetMaterialPath(material->id));
		
			}
			ImGui::End();

			return changeStatus;
	}

	ChangeStatus DrawAnimationControl(const std::string& label, Animation** animation, float size)
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

		if (changed) return ChangeStatus::CHANGED;

		return ChangeStatus::NO_CHANGE;
	}

	ChangeStatus DrawShaderControl(const std::string& label, Shader** shader, float size)
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

		if (changed) return ChangeStatus::CHANGED;

		return ChangeStatus::NO_CHANGE;
	}

	ChangeStatus DrawReflectedMember(void* object, MemberData& member)
	{
		ChangeStatus changeStatus = ChangeStatus::NO_CHANGE;

		byte* obj = (byte*) object;
		
		if (member.type == Types::ID)
			changeStatus = DrawIdControl(member.name.c_str(), *(UUID*)(obj + member.offset));

		if (member.type == Types::FLOAT)
			changeStatus = DrawFloatControl(member.name.c_str(), *(float*)(obj + member.offset));

		if (member.type == Types::FLOAT_ANGLE)
		{
			float temp = glm::degrees(*(float*)(obj + member.offset));
			changeStatus = DrawFloatControl(member.name.c_str(), temp, 0.0f);
			*(float*)(obj + member.offset) = glm::radians(temp);
		}

		if (member.type == Types::FLOAT_NORMALIZED)
			changeStatus = DrawFloatSliderControl(member.name.c_str(), *(float*)(obj + member.offset));

		if (member.type == Types::BOOL)
			changeStatus = DrawBoolControl(member.name.c_str(), *(bool*)(obj + member.offset));

		if (member.type == Types::STRING)
			changeStatus = DrawStringControl(member.name.c_str(), *(std::string*)(obj + member.offset));

		if (member.type == Types::VECTOR3)
			changeStatus = DrawVec3Control(member.name.c_str(), *(glm::vec3*)(obj + member.offset), 0.0f);

		if (member.type == Types::VECTOR3_ANGLES)
		{
			glm::vec3 temp = glm::degrees(*(glm::vec3*)(obj + member.offset));
			changeStatus = DrawVec3Control(member.name.c_str(), temp, 0.0f);
			*(glm::vec3*)(obj + member.offset) = glm::radians(temp);
		}

		if (member.type == Types::VECTOR3_COLOR)
			changeStatus = DrawColorControl(member.name.c_str(), *(glm::vec3*)(obj + member.offset));

		if (member.type == Types::TEXTURE)
		{
			Texture** t = (Texture**)(obj + member.offset);
			changeStatus = DrawTextureControl(member.name.c_str(), t);
		}

		if (member.type == Types::MESH)
		{
			Mesh** m = (Mesh**)(obj + member.offset);
			changeStatus = DrawMeshControl(member.name.c_str(), m);
		}

		if (member.type == Types::SKINNED_MESH)
		{
			SkinnedMesh** m = (SkinnedMesh**)(obj + member.offset);
			changeStatus = DrawSkinnedMeshControl(member.name.c_str(), m);
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
				changeStatus = DrawMaterialEditor("Material Editor", *m, &open);
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
				changeStatus = DrawMaterialEditor("Material Editor", v->at(selected), &open);
		}

		if (member.type == Types::CUBEMAP)
		{
			HdrCubemap** c = (HdrCubemap**)(obj + member.offset);
			changeStatus = DrawCubemapControl(member.name.c_str(), c);
		}

		if (member.type == Types::ANIMATION)
		{
			Animation** a = (Animation**)(obj + member.offset);
			changeStatus = DrawAnimationControl(member.name.c_str(), a);
		}

		return changeStatus;
	}

	ChangeData DrawMetaType(void* object, MetaType& type)
	{
		ChangeData res;
		res.status = ChangeStatus::NO_CHANGE;

		for (MemberData& member : type.members)
		{
			ChangeStatus status;
			byte oldValue[50];

			memcpy(oldValue, (byte*)object + member.offset, member.size);
			status = DrawReflectedMember(object, member);

			if (res.status == ChangeStatus::CHANGING) continue;
			if (status == ChangeStatus::CHANGED)
			{
				res.status = ChangeStatus::CHANGED;
				memcpy(res.oldValue, oldValue, sizeof(res.oldValue));
				memcpy(res.newValue, (byte*)object + member.offset, member.size);

				res.modifiedMember = member;
			}
		}

		if ((int)res.status && type.OnChange) type.OnChange(object);

		return res;
	}
}