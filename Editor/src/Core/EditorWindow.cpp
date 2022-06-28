#include "EditorWindow.h"

#include "../Editor.h"
#include "../Panels/Panels.h"

#include "../Systems/EditorCameraControlSystem.h"
#include "../Utils/StringUtils.h"
#include "../Utils/Dialogs.h"

namespace Seidon
{
	EditorWindow::EditorWindow(Editor& editor) : editor(editor) {}

	void EditorWindow::Init()
	{
		panels.push_back(new AnimationPanel(editor));
		panels.push_back(new ConsolePanel(editor));
		panels.push_back(new HierarchyPanel(editor));
		panels.push_back(new SystemsPanel(editor));
		panels.push_back(new InspectorPanel(editor));
        panels.push_back(new StatisticsPanel(editor));
		panels.push_back(new ViewportPanel(editor));
		panels.push_back(new FileBrowserPanel(editor));

		for (Panel* p : panels)
			p->Init();
	}

	void EditorWindow::Draw()
	{
		dockspace.Begin();

        DrawMenuBar();
        ProcessInput();
	
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

    void EditorWindow::ProcessInput()
    {
        InputManager& inputManager = *editor.GetInputManager();

        if (inputManager.GetKeyPressed(GET_KEYCODE(BACKSLASH)))
            editor.GetWindow()->ToggleFullscreen();

        if (inputManager.GetKeyPressed(GET_KEYCODE(ESCAPE)))
            editor.GetWindow()->ToggleMouseCursor();

        if (inputManager.GetKeyDown(GET_KEYCODE(LEFT_CONTROL)) && inputManager.GetKeyPressed(GET_KEYCODE(B)))
            editor.ReloadExtensions();

        if (inputManager.GetKeyDown(GET_KEYCODE(LEFT_CONTROL)) && inputManager.GetKeyDown(GET_KEYCODE(LEFT_SHIFT)) && inputManager.GetKeyPressed(GET_KEYCODE(B)))
            editor.ReloadExtensionsAndSystems();

        if (inputManager.GetKeyDown(GET_KEYCODE(LEFT_CONTROL)) && inputManager.GetKeyPressed(GET_KEYCODE(S)))
            SaveSceneAndProject();

        if (inputManager.GetKeyDown(GET_KEYCODE(LEFT_CONTROL)) && inputManager.GetKeyDown(GET_KEYCODE(LEFT_SHIFT)) && inputManager.GetKeyPressed(GET_KEYCODE(S)))
            SaveSceneAs();

        if (inputManager.GetKeyDown(GET_KEYCODE(LEFT_CONTROL)) && inputManager.GetKeyPressed(GET_KEYCODE(O)))
            OpenScene();

        if (inputManager.GetKeyDown(GET_KEYCODE(LEFT_CONTROL)) && inputManager.GetKeyPressed(GET_KEYCODE(N)))
            NewScene();
    }

	void EditorWindow::DrawMenuBar()
	{
        static bool openRegistry;
        if (ImGui::BeginMenuBar())
        {
            if (ImGui::BeginMenu("File"))
            {
                if (ImGui::MenuItem("New Scene", "Ctrl+N"))
                    NewScene();

                if (ImGui::MenuItem("Open Scene", "Ctrl+O"))
                    OpenScene();

                if (ImGui::MenuItem("Save Scene As...", "Ctrl+Shift+S"))
                    SaveSceneAs();

                if (ImGui::MenuItem("Save Project and Scene", "Ctrl+S"))
                    SaveSceneAndProject();

                //if (ImGui::MenuItem("Close Project")) 
                //    editor.SwitchProject(nullptr);

                if (ImGui::MenuItem("Exit")) editor.GetWindow()->Close();

                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Extensions"))
            {
                if (ImGui::MenuItem("Extension Registry"))
                    openRegistry = true;

                if (ImGui::MenuItem("Quick Reload", "Ctrl+B"))
                    editor.ReloadExtensions();

                if (ImGui::MenuItem("Full Reload", "Ctrl+Shift+B"))
                    editor.ReloadExtensionsAndSystems();

                ImGui::EndMenu();
            }
            
            ImGui::EndMenuBar();
        }
           
        if(openRegistry) ImGui::OpenPopup("Extension Registry");

        ExtensionRegistryPopup(openRegistry);
      
	}

    void EditorWindow::NewScene()
    {
        std::string filepath = SaveFile("Seidon Scene (*.sdscene)\0*.sdscene\0");

        if (filepath.empty()) return;

        size_t separatorIndex = filepath.find_last_of('\\');
        std::string sceneName = filepath.substr(separatorIndex + 1, filepath.size() - separatorIndex);
        sceneName = sceneName.substr(0, sceneName.find_last_of('.'));

        Scene* scene = new Scene(sceneName);

        scene->AddSystem<RenderSystem>();
        scene->AddSystem<EditorCameraControlSystem>();

        scene->Save(filepath);
        
        delete scene;

        LoadScene(filepath);
    }

    void EditorWindow::LoadScene(const std::string& path)
    {
        Scene* newScene = new Scene();
        newScene->Load(path);

        editor.GetResourceManager()->RegisterAsset(newScene, path);

        editor.SwitchActiveScene(newScene);
    }

    void EditorWindow::OpenScene()
    {
        std::string filepath = LoadFile("Seidon Scene (*.sdscene)\0*.sdscene\0");

        if (filepath.empty()) return;

        LoadScene(filepath);
    }

    void EditorWindow::SaveSceneAndProject()
    {
        std::string scenePath = editor.GetResourceManager()->GetAssetPath(editor.openProject->loadedScene->id);
        editor.openProject->loadedScene->Save(scenePath);

        editor.openProject->Save(editor.openProject->path);
    }

    void EditorWindow::SaveSceneAs()
    {
        std::string filepath = SaveFile("Seidon Scene (*.sdscene)\0*.sdscene\0");

        if (filepath.empty()) return;

        editor.openProject->loadedScene->Save(filepath);
    }

    void EditorWindow::ExtensionRegistryPopup(bool& open)
    {
        ImVec2 center = ImGui::GetMainViewport()->GetCenter();
        ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

        if (ImGui::BeginPopupModal("Extension Registry"))
        {
            ImGui::Text("Loaded Extensions");

            ImGui::PushStyleColor(ImGuiCol_ChildBg, 0xff222222);

            ImGui::BeginChild("Extension List", { ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y * 0.70f });

            int i = 0;
            int toDelete = -1;
            for (Extension* extension : editor.openProject->loadedExtensions)
            {
                ImGui::Selectable(extension->name.c_str());          

                if (ImGui::BeginPopupContextItem())
                {
                    if (ImGui::Button("Unload"))
                    {
                        toDelete = i;
                    }
                    ImGui::EndPopup();
                }

                i++;
            }

            if (toDelete != -1)
            {
                editor.openProject->loadedExtensions[toDelete]->Destroy();
                delete editor.openProject->loadedExtensions[toDelete];

                editor.openProject->loadedExtensions.erase(editor.openProject->loadedExtensions.begin() + toDelete);
                editor.ReloadEditorSystems();
                editor.ReloadActiveScene();
            }

            ImGui::EndChild();
            
            ImGui::PopStyleColor();

            ImGuiStyle& style = ImGui::GetStyle();
            float size = ImGui::CalcTextSize("Load Extension").x + style.FramePadding.x * 2.0f;

            float offset = (ImGui::GetContentRegionAvail().x - size) * 0.5;
            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + offset);

            if (ImGui::Button("Load Extension"))
            {
                std::string path = LoadFile("Seidon Extension (*.dll)\0*.dll\0");

                std::string hotswapPath = editor.openProject->rootDirectory
                    + "\\Temp\\Hotswap\\" + GetNameFromPath(path);

                bool success = false;
                try
                {
                    std::filesystem::copy_file(path, hotswapPath, std::filesystem::copy_options::overwrite_existing);
                    success = true;
                }
                catch (...)
                {
                    std::cerr << "Could not load extension" << std::endl;
                }

                if (success)
                {
                    Extension* extension = new Extension();
                    extension->Load(hotswapPath);

                    if (editor.GetResourceManager()->IsAssetRegistered(path))
                        extension->id = editor.GetResourceManager()->GetAssetId(path);

                    editor.GetResourceManager()->RegisterAssetId(extension->id, path);

                    editor.openProject->loadedExtensions.push_back(extension);
                }
            }

            ImGui::Separator();

            if (ImGui::Button("Close"))
            {
                ImGui::CloseCurrentPopup();
                open = false;
            }

            ImGui::EndPopup();
        }
    }
}