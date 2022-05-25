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
    Editor::Editor()
        : hierarchyPanel(selectedItem), inspectorPanel(selectedItem), fileBrowserPanel(selectedItem), animationPanel(selectedItem) {}

    void Editor::Init()
	{
        int width, height, channelCount;
        unsigned char* data = stbi_load("Resources/ModelIcon.png", &width, &height, &channelCount, 0);
        window->SetIcon(data, width, height);
        delete data;

        if (std::filesystem::exists("ResourceRegistry.sdreg"))
        {
            std::ifstream in("ResourceRegistry.sdreg", std::ios::in | std::ios::binary);
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

        hierarchyPanel.Init();           
        inspectorPanel.Init();           
        fileBrowserPanel.Init();
        
		window->SetName("Seidon Editor");
        window->SetSize(1280, 720);

        scene = new Scene("Editor Scene");
        auto& rs = scene->AddSystem<RenderSystem>();
        scene->AddSystem<EditorCameraControlSystem>();
        scene->Init();
        sceneManager->SetActiveScene(scene);

        rs.AddMainRenderPassFunction(drawColliders);

        ImGuiIO& io = ImGui::GetIO();
        io.Fonts->AddFontFromFileTTF("Resources/Roboto-Regular.ttf", 18);
        e.Init();

        inputManager->ListenToCursor(false);

        editorResourceManager.LoadMesh("Assets\\Cube.sdmesh");
        editorResourceManager.LoadMesh("Assets\\Cylinder.sdmesh");
        editorResourceManager.LoadMesh("Assets\\Semisphere.sdmesh");
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

            if (inputManager->GetKeyDown(GET_KEYCODE(LEFT_CONTROL)) && inputManager->GetKeyPressed(GET_KEYCODE(Z)))
                if (auto action = actions.Pop())
                {
                    action->Undo();
                    delete action;
                }

            //if (inputManager->GetKeyPressed(GET_KEYCODE(LEFT_CONTROL)) && inputManager->GetKeyPressed(GET_KEYCODE(Z)) && inputManager->GetKeyPressed(GET_KEYCODE(LEFT_SHIFT)))
            //    if (auto action = actions.Pop()) action->Do();
        }

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
    
        ImGui::Begin("Viewport");

        inputManager->BlockInput(!ImGui::IsWindowFocused() && !ImGui::IsWindowHovered());

        if (ImGui::Button("Colliders"))
            colliderRenderingEnabled = !colliderRenderingEnabled;

        ImGui::SameLine();

        if (!isPlaying)
        {
            ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 5));
            ImGuiStyle& style = ImGui::GetStyle();
            float size = ImGui::CalcTextSize("Start").x + style.FramePadding.x * 2.0f;

            float offset = (ImGui::GetContentRegionAvail().x - size) * 0.5;
            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + offset);
            if (ImGui::Button("Start"))
            {
                guizmoOperation = -1;

                runtimeScene = new Scene("Runtime Scene");
                scene->CopyEntities(runtimeScene);
                runtimeSystems.CopySystems(runtimeScene);

                sceneManager->SetActiveScene(runtimeScene);
                selectedItem.type = SelectedItemType::NONE;
                isPlaying = true;
            }

            ImGui::PopStyleVar();
        }
        else
        {
            ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 5));
            ImGuiStyle& style = ImGui::GetStyle();
            float size = ImGui::CalcTextSize("Stop").x + style.FramePadding.x * 2.0f;

            float offset = (ImGui::GetContentRegionAvail().x - size) * 0.5;
            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + offset);
            if (ImGui::Button("Stop"))
            {
                sceneManager->SetActiveScene(scene);
                delete runtimeScene;
                selectedItem.type = SelectedItemType::NONE;
                isPlaying = false;
            }
            ImGui::PopStyleVar();
        }

        ImVec2 viewportMinRegion = ImGui::GetWindowContentRegionMin();
        ImVec2 viewportMaxRegion = ImGui::GetWindowContentRegionMax();
        ImVec2 viewportOffset = ImGui::GetWindowPos();

        glm::vec2 viewportBounds[2];
        viewportBounds[0] = { viewportMinRegion.x + viewportOffset.x, viewportMinRegion.y + viewportOffset.y };
        viewportBounds[1] = { viewportMaxRegion.x + viewportOffset.x, viewportMaxRegion.y + viewportOffset.y };

        auto [mouseX, mouseY] = ImGui::GetMousePos();
        mouseX -= viewportBounds[0].x;
        mouseY -= viewportBounds[0].y;
        mouseY = viewportBounds[1].y - viewportBounds[0].y - mouseY;

        inputManager->SetMousePosition(glm::vec2(mouseX, mouseY));
        
        ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
        
        if (sceneManager->GetActiveScene()->HasSystem<RenderSystem>())
        {
            RenderSystem& renderSystem = sceneManager->GetActiveScene()->GetSystem<RenderSystem>();
            renderSystem.SetRenderToScreen(false);

            if(viewportPanelSize.x > 0 && viewportPanelSize.y > 0)
                renderSystem.ResizeFramebuffer(viewportPanelSize.x, viewportPanelSize.y);

            ImGui::Image((ImTextureID)renderSystem.GetRenderTarget().GetRenderId(), ImVec2{ viewportPanelSize.x, viewportPanelSize.y }, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });
       
            EntityId e = renderSystem.GetMouseSelectedEntity();

            if (sceneManager->GetActiveScene()->GetRegistry().valid(e))
            {
                auto& selection = sceneManager->GetActiveScene()->GetRegistry().get<MouseSelectionComponent>(e);

                if (selection.status == SelectionStatus::CLICKED && !ImGuizmo::IsUsing())
                {
                    selectedItem.type = SelectedItemType::ENTITY;
                    selectedItem.entity = Entity(e, sceneManager->GetActiveScene());
                }
            }
        }
        else
        {
            ImGui::Image((ImTextureID)0, ImVec2{ viewportPanelSize.x, viewportPanelSize.y }, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });
        }
        
        if (ImGui::BeginDragDropTarget())
        {
            if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("FILE_BROWSER_SCENE"))
            {
                std::string path = (const char*)payload->Data;
            
                runtimeSystems.Destroy();
                scene->Destroy();

                Scene tempScene;

                std::ifstream in(path, std::ios::in | std::ios::binary);
                tempScene.Load(in);

                tempScene.CopyEntities(scene);
                tempScene.CopySystems(&runtimeSystems);

                scene->AddSystem<RenderSystem>().AddMainRenderPassFunction(drawColliders);
                scene->AddSystem<EditorCameraControlSystem>();

                selectedItem.type = SelectedItemType::NONE;
            }

            if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("FILE_BROWSER_PREFAB"))
            {
                std::string path = (const char*)payload->Data;

                Prefab p;
                p.Load(path);
                
                selectedItem.type = SelectedItemType::ENTITY;
                selectedItem.entity = scene->InstantiatePrefab(p);
            }

            ImGui::EndDragDropTarget();
        }

        DrawTransformGuizmos();
        ImGui::End();

        systemsPanel.Draw();
        inspectorPanel.Draw();
        hierarchyPanel.Draw();
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
	}

	void Editor::Destroy()
	{
        std::ofstream out("ResourceRegistry.sdreg", std::ios::out | std::ios::binary);
        resourceManager->Save(out);

        editorSystems.Destroy();
        runtimeSystems.Destroy();

        e.Destroy();
        e.Unbind();

        editorResourceManager.Destroy();
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

                auto m = editorResourceManager.GetMesh("Assets\\Cube.sdmesh");
                renderer.SubmitMeshWireframe(editorResourceManager.GetMesh("Assets\\Cube.sdmesh"), glm::vec3(0, 1, 0), t1.GetTransformMatrix());
            }
        );
    }

    void Editor::DrawMeshColliders(Renderer& renderer)
    {
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

                    auto m = editorResourceManager.GetMesh("Assets\\Cube.sdmesh");
                    renderer.SubmitMeshWireframe(c.mesh, glm::vec3(0, 1, 0), t1.GetTransformMatrix());
                }
        );
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
                    t1.position = globalTransform.position;

                    float bias = 0.005f;
                    t1.scale = { c.colliderRadius * 2, c.colliderHeight, c.colliderRadius * 2 };
                    t1.scale += glm::vec3(bias);

                    Mesh* semisphere = editorResourceManager.GetMesh("Assets\\Semisphere.sdmesh");
                    Mesh* cylinder = editorResourceManager.GetMesh("Assets\\Cylinder.sdmesh");

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
        if (isPlaying || guizmoOperation == -1) return;

        auto cameras = scene->CreateComponentView<CameraComponent>();

        if (cameras.empty()) return;

        Entity camera = { cameras.front(), scene };

        ImGuizmo::SetOrthographic(false);
        ImGuizmo::SetDrawlist();

        ImGuizmo::SetRect(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y, (float)ImGui::GetWindowWidth(), (float)ImGui::GetWindowHeight());

        TransformComponent& cameraTransform = camera.GetComponent<TransformComponent>();
        CameraComponent& cameraComponent = camera.GetComponent<CameraComponent>();
        glm::mat4 cameraProjection = cameraComponent.GetProjectionMatrix();
        glm::mat4 cameraView = cameraComponent.GetViewMatrix(cameraTransform);

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