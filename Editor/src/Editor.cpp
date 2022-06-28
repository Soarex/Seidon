#include "Editor.h"
#include <EntryPoint.h>

#include "Systems/EditorCameraControlSystem.h"

#include <iostream>
#include <fstream>
#include <filesystem>

namespace Seidon
{
    void Editor::Init()
	{
        int width, height, channelCount;
        unsigned char* data = stbi_load("Resources/ModelIcon.png", &width, &height, &channelCount, 0);
        window->SetIcon(data, width, height);
        delete data;

		window->SetName("Seidon Editor");
        window->SetSize(1280, 720);
        
        ImGuiIO& io = ImGui::GetIO();
        io.Fonts->AddFontFromFileTTF("Resources/Roboto-Regular.ttf", 18);
        
        RegisterSystem<EditorCameraControlSystem>()
            .AddMember("Mouse Sensitivity", &EditorCameraControlSystem::mouseSensitivity)
            .AddMember("Movement Speed", &EditorCameraControlSystem::movementSpeed);

        editorResourceManager.Init();

        editorWindow = new EditorWindow(*this);

        projectSelectionWindow = new ProjectSelectionWindow(*this);
        projectSelectionWindow->Init();
	}

	void Editor::Update()
	{
        if (!openProject)
        {
            openProject = projectSelectionWindow->Draw();

            if (openProject)
            {
                editorWindow->Init();
                activeScene = openProject->loadedScene;
            }
            return;
        }

        editorWindow->Draw();

        if(!isPlaying)
            UpdateEditorSystems();

        if (switchProject)
        {
            openProject->Save(openProject->path);
            delete openProject;

            openProject = projectToSwitch;

            if (openProject)
                activeScene = openProject->loadedScene;

            switchProject = false;
            projectToSwitch = nullptr;
        }
	}

	void Editor::Destroy()
	{
        if(openProject) openProject->Save(openProject->path);
        
        projectSelectionWindow->Destroy();
        editorWindow->Destroy();

        editorResourceManager.Destroy();
	}

    void Editor::Play()
    {
        if (isPlaying) return;

        Scene* runtimeScene = new Scene();
        activeScene = openProject->loadedScene->Duplicate();

        sceneManager->SetActiveScene(activeScene);

        isPlaying = true;
    }

    void Editor::Stop()
    {
        if (!isPlaying) return;

        delete activeScene;
        sceneManager->SetActiveScene(nullptr);

        activeScene = openProject->loadedScene;

        isPlaying = false;
    }

    void Editor::SwitchProject(Project* project)
    {
        switchProject = true;
        projectToSwitch = project;
    }

    void Editor::SwitchActiveScene(Scene* scene)
    {
        delete openProject->loadedScene;

        openProject->loadedScene = scene;
        activeScene = scene;

        ReloadEditorSystems();

        selectedItem.type = SelectedItemType::NONE;
    }

    void Editor::ReloadActiveScene()
    {
        std::string path = resourceManager->GetAssetPath(activeScene->id);

        activeScene->Destroy();
        activeScene->Load(path);
    }

    void Editor::ReloadEditorSystems()
    {
        std::vector<SystemMetaType> systemMetaTypes;

        for (auto& [name, system] : openProject->editorSystems)
            systemMetaTypes.push_back(GetSystemMetaTypeByName(name));

        for (SystemMetaType& metaType : systemMetaTypes)
        {
            openProject->RemoveEditorSystem(metaType);
            openProject->AddEditorSystem(metaType);
        }
    }

    void Editor::ReloadExtensions()
    {
        for (Extension* e : openProject->loadedExtensions)
        {
            std::string path = resourceManager->GetAssetPath(e->id);
            std::string hotswapPath = e->path;

            e->Destroy();

            std::filesystem::copy_file(path, hotswapPath, std::filesystem::copy_options::overwrite_existing);

            e->Load(hotswapPath);
        }
    }

    void Editor::ReloadExtensionsAndSystems()
    {
        ReloadExtensions();
        ReloadEditorSystems();
        ReloadActiveScene();
    }

    void Editor::UpdateEditorSystems()
    {
        openProject->loadedScene->CreateViewAndIterate<TransformComponent>
        (
            [](EntityId id, TransformComponent& transform)
            {
                transform.cacheValid = false;
            }
        );

        for (auto& [name, system] : openProject->editorSystems)
            system->Update(window->GetDeltaTime());
    }

    void Editor::OnEditorAction(EditorAction* action)
    {
        actions.Push(action);

        for (auto& callback : actionCallbacks)
            callback(*action);
    }

    void Editor::AddEditorActionCallback(const EditorActionCallback& callback)
    {
        actionCallbacks.push_back(callback);
    }

    Application* Seidon::CreateApplication() 
    {
        return new Editor();
    }
}