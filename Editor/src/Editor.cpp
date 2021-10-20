#include <Seidon.h>
#include <EntryPoint.h>
#include <glm/glm.hpp>
#include <ImGuizmo/ImGuizmo.h>
#include <filesystem>

#include "Panels/InspectorPanel.h"
#include "Panels/HierarchyPanel.h"
#include "Panels/AssetBrowserPanel.h"
#include "Dockspace.h"
#include "SceneSerializer.h"
#include "Extensions/Extension.h"

#include "EditorCameraControlSystem.h"

using namespace Seidon;

#include <Windows.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>
std::string SaveFile(const char* filter)
{
    OPENFILENAMEA ofn;
    CHAR szFile[260] = { 0 };
    CHAR currentDir[256] = { 0 };
    ZeroMemory(&ofn, sizeof(OPENFILENAME));
    ofn.lStructSize = sizeof(OPENFILENAME);
    ofn.hwndOwner = glfwGetWin32Window((GLFWwindow*)Application::Get()->GetWindow()->GetHandle());
    ofn.lpstrFile = szFile;
    ofn.nMaxFile = sizeof(szFile);
    if (GetCurrentDirectoryA(256, currentDir))
        ofn.lpstrInitialDir = currentDir;
    ofn.lpstrFilter = filter;
    ofn.nFilterIndex = 1;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT | OFN_NOCHANGEDIR;

    // Sets the default extension by extracting it from the filter
    ofn.lpstrDefExt = strchr(filter, '\0') + 1;

    if (GetSaveFileNameA(&ofn) == TRUE)
        return ofn.lpstrFile;

    return std::string();
}

class Editor : public Application
{
public:
    Scene* scene;
    Entity selectedEntity;

    HierarchyPanel hierarchyPanel;
    InspectorPanel inspectorPanel;
    AssetBrowserPanel assetBrowserPanel;
    Dockspace dockspace;
    Extension e;

    Entity camera;

    void Init() override
	{
        e.Bind(L"../Bin/Debug-x64/GameDll/GameDll.dll");
      
        hierarchyPanel.Init();
        inspectorPanel.Init();
        assetBrowserPanel.Init();
        
		window->SetName("Seidon Editor");
        window->SetSize(1280, 720);

        scene = new Scene("Main Scene");
        
        ModelImporter importer;
        ModelImportData importData = importer.Import("Assets/untitled.fbx");
        std::vector<Mesh*> meshes = resourceManager->CreateFromImportData(importData);

        int i = 0;
        for (Mesh* mesh : meshes)
        {
             Entity e = scene->CreateEntity(mesh->name);
             e.GetComponent<TransformComponent>().SetFromMatrix(importData.localTransforms[i]);
             e.AddComponent<RenderComponent>(mesh);
             e.AddComponent<CubeColliderComponent>();
             e.AddComponent<RigidbodyComponent>();

             if(mesh->name != "Floor")
                 e.GetComponent<RigidbodyComponent>().mass = 1;
             else
                 e.GetComponent<CubeColliderComponent>().halfExtents = { 100, 0.5f, 100 };
             i++;
        }
        
        Entity light = scene->CreateEntity("Light");
        light.AddComponent<DirectionalLightComponent>(glm::vec3(1.0f, 1.0f, 1.0f), 1.0f);
        light.GetComponent<TransformComponent>().rotation = glm::vec3(1.0f, 4.0f, 1.0f);

        camera = scene->CreateEntity("Camera");
        camera.AddComponent<CameraComponent>().farPlane = 300;
        camera.EditComponent<TransformComponent>([](TransformComponent& t)
            {
                t.position = { 0, 10, 50 };
                t.rotation = { 0, 0, 0 };
            });

        HdrCubemap* cubemap = new HdrCubemap();
        cubemap->LoadFromEquirectangularMap("Assets/Cubemap.hdr");

        Entity cubemapEntity = scene->CreateEntity("Cubemap");
        cubemapEntity.AddComponent<CubemapComponent>(cubemap);

        scene->AddSystem<EditorCameraControlSystem>(10, 0.5).SetRotationEnabled(true);
        scene->AddSystem<RenderSystem>();
        scene->AddSystem<PhysicSystem>();

        sceneManager->SetActiveScene(scene);

        selectedEntity = { entt::null, nullptr };
        hierarchyPanel.AddSelectionCallback([&](Entity& entity)
            {
                selectedEntity = entity;
            });

        ImGuiIO& io = ImGui::GetIO();
        io.Fonts->AddFontFromFileTTF("Assets/Roboto-Regular.ttf", 18);
        e.Init();

        for (auto& [key, value] : registeredSystems)
            value(*scene);
	}

    int guizmoOperation = ImGuizmo::OPERATION::TRANSLATE;
    bool local = false;

	void Update() override
	{
        RenderSystem& renderSystem = scene->GetSystem<RenderSystem>();
        EditorCameraControlSystem& cameraControlSystem = scene->GetSystem<EditorCameraControlSystem>();

        if (inputManager->GetKeyPressed(GET_KEYCODE(BACKSLASH)))
            window->ToggleFullscreen();

        if (inputManager->GetKeyPressed(GET_KEYCODE(Q)))
            guizmoOperation = -1;

        if (inputManager->GetKeyPressed(GET_KEYCODE(W)) && !inputManager->GetMouseButton(MouseButton::RIGHT))
            guizmoOperation = ImGuizmo::OPERATION::TRANSLATE;

        if (inputManager->GetKeyPressed(GET_KEYCODE(E)))
            guizmoOperation = ImGuizmo::OPERATION::ROTATE;

        if (inputManager->GetKeyPressed(GET_KEYCODE(R)))
            guizmoOperation = ImGuizmo::OPERATION::SCALE;

        if (inputManager->GetKeyPressed(GET_KEYCODE(TAB)))
            local = !local;

        dockspace.Begin();

        if (ImGui::BeginMenuBar())
        {
            if (ImGui::BeginMenu("File"))
            {
                if (ImGui::MenuItem("New", "Ctrl+N"))
                    ;// NewScene();

                if (ImGui::MenuItem("Open...", "Ctrl+O"))
                    ;//OpenScene();

                if (ImGui::MenuItem("Save As...", "Ctrl+Shift+S"))
                {
                    std::string filepath = SaveFile("Seidon Scene (*.sdscene)\0*.sdscene\0");
                    if (!filepath.empty())
                    {
                        SceneSerializer serializer;
                        serializer.Save(sceneManager->GetActiveScene(), filepath);
                    }
                }
                    //SaveSceneAs();

                if (ImGui::MenuItem("Exit")) window->Close();
                ImGui::EndMenu();
            }

            ImGui::EndMenuBar();
        }

        ImGui::Begin("Viewport");
        ImVec2 viewportMinRegion = ImGui::GetWindowContentRegionMin();
        ImVec2 viewportMaxRegion = ImGui::GetWindowContentRegionMax();
        ImVec2 viewportOffset = ImGui::GetWindowPos();

        glm::vec2 viewportBounds[2];
        viewportBounds[0] = { viewportMinRegion.x + viewportOffset.x, viewportMinRegion.y + viewportOffset.y };
        viewportBounds[1] = { viewportMaxRegion.x + viewportOffset.x, viewportMaxRegion.y + viewportOffset.y };

        ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
        renderSystem.ResizeFramebuffer(viewportPanelSize.x, viewportPanelSize.y);

        ImGui::Image((ImTextureID)renderSystem.GetRenderTarget().GetId(), ImVec2{ viewportPanelSize.x, viewportPanelSize.y }, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });

        if (ImGui::BeginDragDropTarget())
        {
            if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_SCENE"))
            {
                std::string path = (const char*)payload->Data;
                SceneSerializer serializer;
                scene = serializer.Load(path);

                if (scene)
                {
                    sceneManager->ChangeActiveScene(scene);
                    selectedEntity = { entt::null, nullptr };
                }
            }
            ImGui::EndDragDropTarget();
        }

        if (selectedEntity.ID != entt::null && guizmoOperation != -1)
        {
            ImGuizmo::SetOrthographic(false);
            ImGuizmo::SetDrawlist();

            ImGuizmo::SetRect(viewportBounds[0].x, viewportBounds[0].y, viewportBounds[1].x - viewportBounds[0].x, viewportBounds[1].y - viewportBounds[0].y);

            TransformComponent& cameraTransform = camera.GetComponent<TransformComponent>();
            CameraComponent& cameraComponent = camera.GetComponent<CameraComponent>();
            glm::mat4 cameraProjection = cameraComponent.GetProjectionMatrix();
            glm::mat4 cameraView = cameraComponent.GetViewMatrix(cameraTransform);

            TransformComponent& entityTransform = selectedEntity.GetComponent<TransformComponent>();
            glm::mat4 transform = entityTransform.GetTransformMatrix();

            ImGuizmo::Manipulate(glm::value_ptr(cameraView), glm::value_ptr(cameraProjection),
                (ImGuizmo::OPERATION)guizmoOperation, local ? ImGuizmo::LOCAL : ImGuizmo::WORLD, glm::value_ptr(transform),
                nullptr, nullptr);

            if (ImGuizmo::IsUsing())
            {
                glm::vec3 position, rotation, scale;
                DecomposeTransform(transform, position, rotation, scale);

                glm::vec3 deltaRotation = rotation - entityTransform.rotation;
                entityTransform.position = position;
                entityTransform.rotation += deltaRotation;
                entityTransform.scale = scale;
            }
        }

        ImGui::End();

        hierarchyPanel.Draw();

        inspectorPanel.SetSelectedEntity(selectedEntity);
        inspectorPanel.Draw();
       
        assetBrowserPanel.Draw();

        ImGui::Begin("Stats"); 
        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
        ImGui::End();

        
        dockspace.End();
	}

	void Destroy() override
	{
        e.Destroy();
        e.Unbind();
	}
};

Application* Seidon::CreateApplication() 
{
    return new Editor();
}