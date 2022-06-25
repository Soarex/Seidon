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
        
        ImGui::PushID("Scene Systems");
		ImGui::Text("Scene Systems");
        ImGui::Separator();
        
        DrawSceneSystems(editor.activeScene);

        ImGui::PopID();

        ImGui::PushID("Editor Systems");
        ImGui::Text("Editor Systems");
        ImGui::Separator();

        DrawEditorSystems();

        ImGui::PopID();
		ImGui::End();
	}

    void SystemsPanel::DrawSceneSystems(Scene* scene)
    {
        ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_FramePadding;
        for (auto& metaType : editor.GetSystemsMetaTypes())
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
            for (auto& metaType : editor.GetSystemsMetaTypes())
            {
                if (!metaType.Has(*scene) && ImGui::MenuItem(metaType.name.c_str() + 6))
                    metaType.Add(*scene);
            }

            ImGui::EndPopup();
        }
    }

    void SystemsPanel::DrawEditorSystems()
    {
        ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_FramePadding;

        std::string nameToErase = "";
        bool toErase = false;
        for (auto& [name, system] : editor.openProject->editorSystems)
        {
            ImGui::PushID(name.c_str());

            bool open = ImGui::CollapsingHeader(name.c_str() + 6, flags);

            if (ImGui::BeginPopupContextItem())
            {
                if (ImGui::MenuItem("Remove System"))
                {
                    toErase = true;
                    nameToErase = name;
                }

                ImGui::EndPopup();
                ImGui::PopID();
                continue;
            }

            if (open)
            {
                ImGui::Indent();

                MetaType metaType = editor.GetSystemMetaTypeByName(name);
                DrawMetaType(system, metaType);

                ImGui::Unindent();
            }

            ImGui::PopID();
        }

        if (toErase)
        {
            SystemMetaType t = editor.GetSystemMetaTypeByName(nameToErase);
            editor.openProject->RemoveEditorSystem(t);
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
            for (auto& metaType : editor.GetSystemsMetaTypes())
            {
                if (editor.openProject->editorSystems.count(metaType.name) > 0) continue;

                if (ImGui::MenuItem(metaType.name.c_str() + 6))
                    editor.openProject->AddEditorSystem(metaType);
            }

            ImGui::EndPopup();
        }
    }
}