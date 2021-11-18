#include "SystemsPanel.h"
#include "../Utils/DrawFunctions.h"

#include <Seidon.h>
#include "../Editor.h"

namespace Seidon
{
	void SystemsPanel::Init()
	{

	}

	void SystemsPanel::Draw()
	{
		ImGui::Begin("Systems");
        Editor* editor = (Editor*)Application::Get();
		Scene* currentScene = Application::Get()->GetSceneManager()->GetActiveScene();

        Scene *runtimeSystems, *editorSystems;
        if (!editor->isPlaying)
        {
            runtimeSystems = &editor->runtimeSystems;
            editorSystems = currentScene;
        }
        else
        {
            runtimeSystems = currentScene;
            editorSystems = editor->scene;
        }

        ImGui::PushID("Scene Systems");
		ImGui::Text("Scene Systems");
		for (auto& metaType : Application::Get()->GetSystemsMetaTypes())
		{
			ImGui::PushID(metaType.name.c_str());
            if (metaType.Has(*runtimeSystems) && ImGui::CollapsingHeader(metaType.name.c_str() + 6, ImGuiTreeNodeFlags_DefaultOpen))
            {
                if (ImGui::Button("X"))
                {
                    metaType.Delete(*runtimeSystems);
                    ImGui::PopID();
                    continue;
                }

                char* system = (char*)metaType.Get(*runtimeSystems);

                for (const MemberData& member : metaType.members)
                {
                    if (member.type == Types::FLOAT)
                        DrawFloatControl(member.name.c_str(), *(float*)(system + member.offset));

                    if (member.type == Types::VECTOR3)
                        DrawVec3Control(member.name.c_str(), *(glm::vec3*)(system + member.offset), 0.0f);

                    if (member.type == Types::VECTOR3_ANGLES)
                    {
                        glm::vec3 temp = glm::degrees(*(glm::vec3*)(system + member.offset));
                        DrawVec3Control(member.name.c_str(), temp, 0.0f);
                        *(glm::vec3*)(system + member.offset) = glm::radians(temp);
                    }

                    if (member.type == Types::VECTOR3_COLOR)
                        DrawColorControl(member.name.c_str(), *(glm::vec3*)(system + member.offset));

                    if (member.type == Types::TEXTURE)
                    {
                        Texture* t = *(Texture**)(system + member.offset);
                        DrawTextureControl(member.name.c_str(), t);
                    }

                    if (member.type == Types::MESH)
                    {
                        Mesh** m = (Mesh**)(system + member.offset);
                        DrawMeshControl(member.name.c_str(), m);
                    }

                    if (member.type == Types::MATERIAL_VECTOR)
                    {
                        std::vector<Material*>* v = (std::vector<Material*>*)(system + member.offset);

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
                        HdrCubemap** c = (HdrCubemap**)(system + member.offset);
                        DrawCubemapControl(member.name.c_str(), c);
                    }
                }
            }
            ImGui::PopID();
		}

		ImGui::Spacing();
		ImGui::Spacing();

		if (ImGui::Button("Add System"))
			ImGui::OpenPopup("AddSceneSystem");

		if (ImGui::BeginPopup("AddSceneSystem"))
		{
			for (auto& metaType : Application::Get()->GetSystemsMetaTypes())
			{
				if (!metaType.Has(*runtimeSystems) && ImGui::MenuItem(metaType.name.c_str() + 6))
					metaType.Add(*runtimeSystems);
			}

			ImGui::EndPopup();
		}
        ImGui::PopID();

        ImGui::PushID("Editor Systems");
        ImGui::Text("Editor Systems");
        for (auto& metaType : Application::Get()->GetSystemsMetaTypes())
        {
            ImGui::PushID(metaType.name.c_str());
            if (metaType.Has(*editorSystems) && ImGui::CollapsingHeader(metaType.name.c_str() + 6, ImGuiTreeNodeFlags_DefaultOpen))
            {
                if (ImGui::Button("X"))
                {
                    metaType.Delete(*editorSystems);
                    ImGui::PopID();
                    continue;
                }

                char* system = (char*)metaType.Get(*editorSystems);

                for (const MemberData& member : metaType.members)
                {
                    if (member.type == Types::FLOAT)
                        DrawFloatControl(member.name.c_str(), *(float*)(system + member.offset));

                    if (member.type == Types::VECTOR3)
                        DrawVec3Control(member.name.c_str(), *(glm::vec3*)(system + member.offset), 0.0f);

                    if (member.type == Types::VECTOR3_ANGLES)
                    {
                        glm::vec3 temp = glm::degrees(*(glm::vec3*)(system + member.offset));
                        DrawVec3Control(member.name.c_str(), temp, 0.0f);
                        *(glm::vec3*)(system + member.offset) = glm::radians(temp);
                    }

                    if (member.type == Types::VECTOR3_COLOR)
                        DrawColorControl(member.name.c_str(), *(glm::vec3*)(system + member.offset));

                    if (member.type == Types::TEXTURE)
                    {
                        Texture* t = *(Texture**)(system + member.offset);
                        DrawTextureControl(member.name.c_str(), t);
                    }

                    if (member.type == Types::MESH)
                    {
                        Mesh** m = (Mesh**)(system + member.offset);
                        DrawMeshControl(member.name.c_str(), m);
                    }

                    if (member.type == Types::MATERIAL_VECTOR)
                    {
                        std::vector<Material*>* v = (std::vector<Material*>*)(system + member.offset);

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
                        HdrCubemap** c = (HdrCubemap**)(system + member.offset);
                        DrawCubemapControl(member.name.c_str(), c);
                    }
                }
            }
            ImGui::PopID();
        }

        ImGui::Spacing();
        ImGui::Spacing();

        if (ImGui::Button("Add System"))
            ImGui::OpenPopup("AddEditorSystem");

        if (ImGui::BeginPopup("AddEditorSystem"))
        {
            for (auto& metaType : Application::Get()->GetSystemsMetaTypes())
            {
                if (!metaType.Has(*editorSystems) && ImGui::MenuItem(metaType.name.c_str() + 6))
                    metaType.Add(*editorSystems);
            }

            ImGui::EndPopup();
        }
        ImGui::PopID();
		ImGui::End();
	}
}