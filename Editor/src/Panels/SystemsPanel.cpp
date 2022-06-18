#include "SystemsPanel.h"
#include "../Utils/DrawFunctions.h"

#include "../Editor.h"

namespace Seidon
{
	void SystemsPanel::Init()
	{

	}

	void SystemsPanel::Draw()
	{
        if (!ImGui::Begin("Systems"))
        {
            ImGui::End();
            return;
        }

		Scene* currentScene = editor.GetSceneManager()->GetActiveScene();

        Scene *runtimeSystems, *editorSystems;
        if (!editor.isPlaying)
        {
            runtimeSystems = &editor.runtimeSystems;
            editorSystems = currentScene;
        }
        else
        {
            runtimeSystems = currentScene;
            editorSystems = editor.scene;
        }
        
        ImGui::PushID("Scene Systems");
		ImGui::Text("Scene Systems");
        ImGui::Separator();
        
        DrawSystems(runtimeSystems);

        ImGui::PopID();

        ImGui::PushID("Editor Systems");
        ImGui::Text("Editor Systems");
        ImGui::Separator();

        DrawSystems(editorSystems);

        ImGui::PopID();
		ImGui::End();
	}

    void SystemsPanel::DrawSystems(Scene* scene)
    {
        ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_FramePadding;
        for (auto& metaType : Application::Get()->GetSystemsMetaTypes())
        {
            if (!metaType.Has(*scene)) continue;
            ImGui::PushID(metaType.name.c_str());

            bool open = ImGui::CollapsingHeader(metaType.name.c_str() + 6, flags);

            if (ImGui::BeginPopupContextItem())
            {
                if (ImGui::MenuItem("Remove System"))
                    metaType.Delete(*scene);

                ImGui::EndPopup();
                ImGui::PopID();
                continue;
            }

            if (open)
            {
                ImGui::Indent();

                DrawMetaType(metaType.Get(*scene), metaType);
                
                ImGui::Unindent();
            }

            ImGui::PopID();
        }

        ImGui::Spacing();

        ImGuiStyle& style = ImGui::GetStyle();
        float size = ImGui::CalcTextSize("Add System").x + style.FramePadding.x * 2.0f;

        float offset = (ImGui::GetContentRegionAvail().x - size) * 0.5;
        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + offset);

        if (ImGui::Button("Add System"))
            ImGui::OpenPopup("AddSystem");

        if (ImGui::BeginPopup("AddSystem"))
        {
            for (auto& metaType : Application::Get()->GetSystemsMetaTypes())
            {
                if (!metaType.Has(*scene) && ImGui::MenuItem(metaType.name.c_str() + 6))
                    metaType.Add(*scene);
            }

            ImGui::EndPopup();
        }
    }
}