#include "Editor.h"
#include <EntryPoint.h>

#include <glm/glm.hpp>

#include "SceneSerializer.h"
#include "Utils/Dialogs.h"
#include "EditorCameraControlSystem.h"

namespace Seidon
{
    void Editor::Init()
	{
        int width, height, channelCount;
        unsigned char* data = stbi_load("Assets/ModelIcon.png", &width, &height, &channelCount, 0);
        window->SetIcon(data, width, height);
        delete data;
        RegisterSystem<RenderSystem>(); 
        RegisterSystem<EditorCameraControlSystem>()
            .AddMember("Mouse Sensitivity", &EditorCameraControlSystem::mouseSensitivity)
            .AddMember("Movement Speed", &EditorCameraControlSystem::movementSpeed);
        RegisterSystem<PhysicSystem>();

#ifdef NDEBUG
        e.Bind(L"../Bin/Release-x64/GameDll/GameDll.dll");
#else
        e.Bind(L"../Bin/Debug-x64/GameDll/GameDll.dll");
#endif

        hierarchyPanel.Init();           
        inspectorPanel.Init();           
        assetBrowserPanel.Init();
        
		window->SetName("Seidon Editor");
        window->SetSize(1280, 720);
        RenderSystem* er = new RenderSystem();
        scene = new Scene("Main Scene");

        sceneManager->SetActiveScene(scene);

        selectedEntity = { entt::null, nullptr };
        hierarchyPanel.AddSelectionCallback([&](Entity& entity)
            {
                selectedEntity = entity;
            });

        ImGuiIO& io = ImGui::GetIO();
        io.Fonts->AddFontFromFileTTF("Assets/Roboto-Regular.ttf", 18);
        e.Init();
	}

	void Editor::Update()
	{
        if (inputManager->GetKeyPressed(GET_KEYCODE(BACKSLASH)))
            window->ToggleFullscreen();

        if (inputManager->GetKeyPressed(GET_KEYCODE(ESCAPE)))
            window->ToggleMouseCursor();

        if (!isPlaying)
        {
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
        }

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

                if (ImGui::MenuItem("Exit")) window->Close();
                ImGui::EndMenu();
            }

            ImGui::EndMenuBar();
        }
    
        ImGui::Begin("Viewport");

        inputManager->BlockInput(!ImGui::IsWindowFocused() && !ImGui::IsWindowHovered());

        if (!isPlaying)
        {
            if (ImGui::Button("Start"))
            {
                guizmoOperation = -1;

                runtimeScene = new Scene(scene->GetName());
                scene->CopyEntities(runtimeScene);
                runtimeSystems.CopySystems(runtimeScene);

                sceneManager->SetActiveScene(runtimeScene);
                selectedEntity = { entt::null, nullptr };
                isPlaying = true;
            }
        } 
        else
        {
            if (ImGui::Button("Stop"))
            {
                sceneManager->SetActiveScene(scene);
                delete runtimeScene;
                selectedEntity = { entt::null, nullptr };
                isPlaying = false;
            }
        }

        ImVec2 viewportMinRegion = ImGui::GetWindowContentRegionMin();
        ImVec2 viewportMaxRegion = ImGui::GetWindowContentRegionMax();
        ImVec2 viewportOffset = ImGui::GetWindowPos();

        glm::vec2 viewportBounds[2];
        viewportBounds[0] = { viewportMinRegion.x + viewportOffset.x, viewportMinRegion.y + viewportOffset.y };
        viewportBounds[1] = { viewportMaxRegion.x + viewportOffset.x, viewportMaxRegion.y + viewportOffset.y };
        
        ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
        
        if (sceneManager->GetActiveScene()->HasSystem<RenderSystem>())
        {
            RenderSystem& renderSystem = sceneManager->GetActiveScene()->GetSystem<RenderSystem>();
            renderSystem.ResizeFramebuffer(viewportPanelSize.x, viewportPanelSize.y);

            ImGui::Image((ImTextureID)renderSystem.GetRenderTarget().GetId(), ImVec2{ viewportPanelSize.x, viewportPanelSize.y }, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });
        }
        else
        {
            ImGui::Image((ImTextureID)0, ImVec2{ viewportPanelSize.x, viewportPanelSize.y }, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });
        }
        
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

        if (!isPlaying && selectedEntity.ID != entt::null && guizmoOperation != -1)
        {
            entt::basic_group cameras = scene->GetRegistry().group<CameraComponent, TransformComponent>();

            for (entt::entity e : cameras)
            {
                Entity camera = { e, &scene->GetRegistry() };

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
        }
        ImGui::End();


        inspectorPanel.SetSelectedEntity(selectedEntity);
        inspectorPanel.Draw();
       
        assetBrowserPanel.Draw();
        systemsPanel.Draw();
        hierarchyPanel.Draw();

        ImGui::Begin("Stats"); 
        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
        ImGui::End();

        
        dockspace.End();
	}

	void Editor::Destroy()
	{
        editorSystems.Destroy();
        runtimeSystems.Destroy();
        e.Destroy();
        e.Unbind();
	}


    Application* Seidon::CreateApplication() 
    {
        return new Editor();
    }
}