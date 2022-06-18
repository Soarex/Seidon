#include "Editor.h"
#include <EntryPoint.h>

#include <glm/glm.hpp>
#include <iostream>
#include <fstream>

#include "Utils/Dialogs.h"
#include "EditorCameraControlSystem.h"

#include <Debug/Timer.h>

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
        
        editorWindow = new EditorWindow(*this);

        projectSelectionWindow = new ProjectSelectionWindow(*this);
        projectSelectionWindow->Init();
        /*
        if (std::filesystem::exists("Assets\\ResourceRegistry.sdreg"))
        {
            std::ifstream in("Assets\\ResourceRegistry.sdreg", std::ios::in | std::ios::binary);
            resourceManager->Load(in);
        }

        drawColliders = [&](Renderer& renderer)
        {
            if (!colliderRenderingEnabled) return;

            DrawCubeColliders(renderer);
            DrawMeshColliders(renderer);
            DrawCharacterControllers(renderer);
        };

        editorResourceManager.Init();

        RegisterSystem<EditorCameraControlSystem>()
            .AddMember("Mouse Sensitivity", &EditorCameraControlSystem::mouseSensitivity)
            .AddMember("Movement Speed", &EditorCameraControlSystem::movementSpeed);

#ifdef NDEBUG
        e.Bind(L"../Bin/Release-x64/GameDll/GameDll.dll");
#else
        e.Bind(L"../Bin/Debug-x64/GameDll/GameDll.dll");
#endif


        scene = new Scene("Editor Scene");
        auto& rs = scene->AddSystem<RenderSystem>();
        scene->AddSystem<EditorCameraControlSystem>();
        scene->Init();
        sceneManager->SetActiveScene(scene);

        rs.AddMainRenderPassFunction(drawColliders);


        inputManager->ListenToCursor(false);

        editorResourceManager.LoadAsset<Mesh>("Assets\\Cube.sdmesh");
        editorResourceManager.LoadAsset<Mesh>("Assets\\Cylinder.sdmesh");
        editorResourceManager.LoadAsset<Mesh>("Assets\\Semisphere.sdmesh");

        e.Init();
        */
	}

	void Editor::Update()
	{
        if (!openProject)
        {
            openProject = projectSelectionWindow->Draw();

            if (openProject)
            {
                editorWindow->Init();
                sceneManager->SetActiveScene(openProject->currentScene);
            }
            return;
        }

        editorWindow->Draw();
        /*
        if (inputManager->GetKeyPressed(GET_KEYCODE(BACKSLASH)))
            window->ToggleFullscreen();

        if (inputManager->GetKeyPressed(GET_KEYCODE(ESCAPE)))
            window->ToggleMouseCursor();

        

        dockspace.Begin();

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
                    }
                }

                if (ImGui::MenuItem("Save As...", "Ctrl+Shift+S"))
                {
                    std::string filepath = SaveFile("Seidon Scene (*.sdscene)\0*.sdscene\0");
                    if (!filepath.empty())
                    {
                        std::ofstream out(filepath, std::ios::out | std::ios::binary);
                        Scene tempScene("Temporary Scene");

                        scene->CopyEntities(&tempScene);
                        runtimeSystems.CopySystems(&tempScene);

                        tempScene.Save(out);
                    }
                }

                if (ImGui::MenuItem("Exit")) window->Close();
                ImGui::EndMenu();
            }

            ImGui::EndMenuBar();
        }
    
        DrawTransformGuizmos();

        systemsPanel.Draw();
        inspectorPanel.Draw();
        hierarchyPanel.Draw();
        consolePanel.Draw();
        animationPanel.Draw();

        ImGui::Begin("Stats"); 
        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

        if (sceneManager->GetActiveScene()->HasSystem<RenderSystem>())
        {
            RenderSystem& renderSystem = sceneManager->GetActiveScene()->GetSystem<RenderSystem>();
            const RenderStats& stats = renderSystem.GetRenderStats();

            ImGui::Text("Vertex use: %d / %d", stats.vertexCount, stats.vertexBufferSize);
            ImGui::Text("Index use: %d / %d", stats.indexCount, stats.indexBufferSize);

            float memoryUsedInMB = (stats.indexCount * sizeof(int) + stats.vertexCount * sizeof(Vertex)) / 1000000.0f;
            float memoryAllocatedInMB = (stats.indexBufferSize * sizeof(int) + stats.vertexBufferSize * sizeof(Vertex)) / 1000000.0f;
            ImGui::Text("Memory used: %.2f MB / %.2f MB", memoryUsedInMB, memoryAllocatedInMB);
            ImGui::Text("Object count: %d in %d batches", stats.objectCount, stats.batchCount);
        }

        ImGui::End();

        fileBrowserPanel.Draw();

        dockspace.End();
        */
	}

	void Editor::Destroy()
	{
        /*
        std::ofstream out("Assets\\ResourceRegistry.sdreg", std::ios::out | std::ios::binary);
        resourceManager->Save(out);

        editorSystems.Destroy();
        runtimeSystems.Destroy();

        e.Destroy();
        e.Unbind();

        editorResourceManager.Destroy();
        */
        projectSelectionWindow->Destroy();
        editorWindow->Destroy();
	}

    void Editor::DrawCubeColliders(Renderer& renderer)
    {
        sceneManager->GetActiveScene()->CreateGroupAndIterate<CubeColliderComponent>
        (
            GetTypeList<TransformComponent>,
            [&](EntityId id, CubeColliderComponent& c, TransformComponent& t)
            {
                TransformComponent globalTransform;
                globalTransform.SetFromMatrix(sceneManager->GetActiveScene()->GetEntityByEntityId(id).GetGlobalTransformMatrix());

                TransformComponent t1;
                t1.position = globalTransform.position + c.offset;
                t1.rotation = globalTransform.rotation;

                float bias = 0.005f;
                t1.scale = globalTransform.scale * c.halfExtents * 2.0f + glm::vec3(bias);

                auto m = editorResourceManager.GetAsset<Mesh>("Assets\\Cube.sdmesh");
                renderer.SubmitMeshWireframe(editorResourceManager.GetAsset<Mesh>("Assets\\Cube.sdmesh"), glm::vec3(0, 1, 0), t1.GetTransformMatrix());
            }
        );
    }

    void Editor::DrawMeshColliders(Renderer& renderer)
    {
        /*
        sceneManager->GetActiveScene()->CreateGroupAndIterate<MeshColliderComponent>
        (
                GetTypeList<TransformComponent>,
                [&](EntityId id, MeshColliderComponent& c, TransformComponent& t)
                {
                    TransformComponent globalTransform;
                    globalTransform.SetFromMatrix(sceneManager->GetActiveScene()->GetEntityByEntityId(id).GetGlobalTransformMatrix());

                    TransformComponent t1;
                    t1.position = globalTransform.position;
                    t1.rotation = globalTransform.rotation;

                    float bias = 0.005f;
                    t1.scale = t.scale + glm::vec3(bias);

                    auto m = editorResourceManager.GetAsset<Mesh>("Assets\\Cube.sdmesh");
                    renderer.SubmitMeshWireframe(c.mesh, glm::vec3(0, 1, 0), t1.GetTransformMatrix());
                }
        );
        */
    }

    void Editor::DrawCharacterControllers(Renderer& renderer)
    {
        sceneManager->GetActiveScene()->CreateGroupAndIterate<CharacterControllerComponent>
        (
                GetTypeList<TransformComponent>,
                [&](EntityId id, CharacterControllerComponent& c, TransformComponent& t)
                {
                    TransformComponent globalTransform;
                    globalTransform.SetFromMatrix(sceneManager->GetActiveScene()->GetEntityByEntityId(id).GetGlobalTransformMatrix());

                    TransformComponent t1;
                    t1.position = globalTransform.position + c.offset;

                    float bias = 0.005f;
                    t1.scale = { c.colliderRadius * 2, c.colliderHeight, c.colliderRadius * 2 };
                    t1.scale += glm::vec3(bias);

                    Mesh* semisphere = editorResourceManager.GetAsset<Mesh>("Assets\\Semisphere.sdmesh");
                    Mesh* cylinder = editorResourceManager.GetAsset<Mesh>("Assets\\Cylinder.sdmesh");

                    renderer.SubmitMeshWireframe(cylinder, glm::vec3(0, 1, 0), t1.GetTransformMatrix());

                    t1.scale.y = t1.scale.x;
                    t1.position.y += c.colliderHeight / 2;

                    renderer.SubmitMeshWireframe(semisphere, glm::vec3(0, 1, 0), t1.GetTransformMatrix());

                    t1.position.y -= c.colliderHeight;
                    t1.rotation.x = glm::radians(180.0f);

                    renderer.SubmitMeshWireframe(semisphere, glm::vec3(0, 1, 0), t1.GetTransformMatrix());

                    t1.position.y -= c.contactOffset / 2 + 1 * c.colliderRadius;
                    t1.scale.y = c.contactOffset;

                    renderer.SubmitMeshWireframe(cylinder, glm::vec3(1, 1, 1), t1.GetTransformMatrix());
                }
        );
    }

    void Editor::DrawGuizmos(Renderer& renderer)
    {
        DrawTransformGuizmos();
    }

    void Editor::DrawTransformGuizmos()
    {
        /*
        if (isPlaying || guizmoOperation == -1) return;

        auto cameras = scene->CreateComponentView<CameraComponent>();

        if (cameras.empty()) return;

        Entity camera = { cameras.front(), scene };

        ImGuizmo::SetOrthographic(false);
        ImGuizmo::SetDrawlist();

        ImGuizmo::SetRect(viewBounds.left, viewBounds.top, viewBounds.right - viewBounds.left, viewBounds.bottom - viewBounds.top);

        TransformComponent& cameraTransform = camera.GetComponent<TransformComponent>();
        CameraComponent& cameraComponent = camera.GetComponent<CameraComponent>();
        glm::mat4 cameraProjection;
        glm::mat4 cameraView;
        if (selectedItem.type == SelectedItemType::ENTITY && (selectedItem.entity.HasComponent<UITextComponent>() || selectedItem.entity.HasComponent<UISpriteComponent>()))
        {
            float aspectRatio = cameraComponent.aspectRatio;
            
            float frameHalfSize = 100;
            cameraProjection = glm::ortho(-frameHalfSize * aspectRatio, frameHalfSize * aspectRatio, -frameHalfSize, frameHalfSize, -10.0f, 10.0f);
            cameraView = glm::identity<glm::mat4>();
            ImGuizmo::SetOrthographic(true);
        }
        else
        {
            cameraProjection = cameraComponent.GetProjectionMatrix();
            cameraView = cameraComponent.GetViewMatrix(cameraTransform);
        }

        glm::mat4 transform;
        if (selectedItem.type == SelectedItemType::ENTITY)
        {
            TransformComponent& entityTransform = selectedItem.entity.GetComponent<TransformComponent>();
            transform = selectedItem.entity.GetGlobalTransformMatrix();

            ImGuizmo::Manipulate(glm::value_ptr(cameraView), glm::value_ptr(cameraProjection),
                (ImGuizmo::OPERATION)guizmoOperation, local ? ImGuizmo::LOCAL : ImGuizmo::WORLD, glm::value_ptr(transform),
                nullptr, nullptr);

            if (ImGuizmo::IsUsing())
            {
                glm::vec3 position, rotation, scale;
                if (selectedItem.entity.HasParent())
                    DecomposeTransform(glm::inverse(selectedItem.entity.GetParent().GetGlobalTransformMatrix()) * transform, position, rotation, scale);
                else
                    DecomposeTransform(transform, position, rotation, scale);

                glm::vec3 deltaRotation = rotation - entityTransform.rotation;
                entityTransform.position = position;
                entityTransform.rotation += deltaRotation;
                entityTransform.scale = scale;
            }
        }
        if (selectedItem.type == SelectedItemType::BONE)
        {
            TransformComponent t;

            glm::mat4 parentTransformWorldSpace = glm::mat4(1);
            int parentId = selectedItem.boneData.armature->bones[selectedItem.boneData.id].parentId;
             
            while (parentId != -1)
            {
                parentTransformWorldSpace *= (*selectedItem.boneData.transforms)[parentId];
                parentId = selectedItem.boneData.armature->bones[parentId].parentId;
            }

            transform = selectedItem.boneData.owningEntity.GetGlobalTransformMatrix() * parentTransformWorldSpace * (*selectedItem.boneData.transforms)[selectedItem.boneData.id];
            t.SetFromMatrix(transform);

            ImGuizmo::Manipulate(glm::value_ptr(cameraView), glm::value_ptr(cameraProjection),
                (ImGuizmo::OPERATION)guizmoOperation, local ? ImGuizmo::LOCAL : ImGuizmo::WORLD, glm::value_ptr(transform),
                nullptr, nullptr);

            if (ImGuizmo::IsUsing())
            {
                glm::vec3 position, rotation, scale;
                DecomposeTransform(transform, position, rotation, scale);

                glm::vec3 deltaRotation = rotation - t.rotation;
                t.position = position;
                t.rotation += deltaRotation;
                t.scale = scale;

                (*selectedItem.boneData.transforms)[selectedItem.boneData.id] = glm::inverse(parentTransformWorldSpace) * glm::inverse(selectedItem.boneData.owningEntity.GetGlobalTransformMatrix()) *  t.GetTransformMatrix();
            }

        }
        */
        
    }

    void Editor::ReloadDll()
    {
        e.Destroy();
        e.Unbind();

#ifdef NDEBUG
        e.Bind(L"../Bin/Release-x64/GameDll/GameDll.dll");
#else
        e.Bind(L"../Bin/Debug-x64/GameDll/GameDll.dll");
#endif

        e.Init();
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