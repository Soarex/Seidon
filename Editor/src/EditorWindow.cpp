#include "EditorWindow.h"

#include "Editor.h"
#include "Panels/Panels.h"

#include "Utils/Dialogs.h"

namespace Seidon
{
	EditorWindow::EditorWindow(Editor& editor) : editor(editor) {}

	void EditorWindow::Init()
	{
		this->project = project;

		panels.push_back(new AnimationPanel(editor));
		panels.push_back(new ConsolePanel(editor));
		panels.push_back(new FileBrowserPanel(editor));
		panels.push_back(new HierarchyPanel(editor));
		panels.push_back(new InspectorPanel(editor));
		panels.push_back(new SystemsPanel(editor));
		panels.push_back(new ViewportPanel(editor));

		for (Panel* p : panels)
			p->Init();
	}

	void EditorWindow::Draw()
	{
        DrawMenuBar();

		dockspace.Begin();

		for (Panel* p : panels)
			p->Draw();

		dockspace.End();
	}

	void EditorWindow::Destroy()
	{
		for (Panel* p : panels)
		{
			p->Destroy();
			delete p;
		}

		panels.clear();
	}

	void EditorWindow::DrawMenuBar()
	{
        if (ImGui::BeginMenuBar())
        {
            if (ImGui::BeginMenu("File"))
            {
                if (ImGui::MenuItem("New", "Ctrl+N"))
                    ;// NewScene();

                if (ImGui::MenuItem("Open...", "Ctrl+O"))
                {
                    std::string filepath = LoadFile("Seidon Scene (*.sdscene)\0*.sdscene\0");
                    if (!filepath.empty())
                    {
                        /*
                        runtimeSystems.Destroy();
                        scene->Destroy();

                        Scene tempScene("Temporary Scene");

                        std::ifstream in(filepath, std::ios::in | std::ios::binary);
                        tempScene.Load(in);

                        tempScene.CopyEntities(scene);
                        tempScene.CopySystems(&runtimeSystems);

                        scene->AddSystem<RenderSystem>().AddMainRenderPassFunction(drawColliders);
                        scene->AddSystem<EditorCameraControlSystem>();

                        selectedItem.type = SelectedItemType::NONE;
                        */
                    }
                }

                if (ImGui::MenuItem("Save As...", "Ctrl+Shift+S"))
                {
                    std::string filepath = SaveFile("Seidon Scene (*.sdscene)\0*.sdscene\0");
                    if (!filepath.empty())
                    {
                        /*
                        std::ofstream out(filepath, std::ios::out | std::ios::binary);
                        Scene tempScene("Temporary Scene");

                        scene->CopyEntities(&tempScene);
                        runtimeSystems.CopySystems(&tempScene);

                        tempScene.Save(out);
                        */
                    }
                }

                if (ImGui::MenuItem("Exit")) editor.GetWindow()->Close();
                ImGui::EndMenu();
            }

            ImGui::EndMenuBar();
        }
	}
}