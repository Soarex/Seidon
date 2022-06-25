#include "ViewportPanel.h"

#include "../Editor.h"

namespace Seidon
{
	void ViewportPanel::Init()
	{
        editor.GetInputManager()->ListenToCursor(false);
	}

	void ViewportPanel::Draw()
	{
        if (!ImGui::Begin("Viewport"))
        {
            ImGui::End();
            return;
        }

        InputManager& inputManager = *editor.GetInputManager();

        inputManager.BlockInput(!ImGui::IsWindowFocused() && !ImGui::IsWindowHovered());


        if (ImGui::Button("Colliders"));
            //colliderRenderingEnabled = !colliderRenderingEnabled;

        ImGui::SameLine();

        if (!editor.isPlaying)
        {
            ProcessInput();

            ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 5));
            ImGuiStyle& style = ImGui::GetStyle();
            float size = ImGui::CalcTextSize("Start").x + style.FramePadding.x * 2.0f;

            float offset = (ImGui::GetContentRegionAvail().x - size) * 0.5;
            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + offset);

            if (ImGui::Button("Start"))
            {
                guizmoOperation = -1;

                editor.Play();
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
                editor.Stop();

            ImGui::PopStyleVar();
        }

        ImVec2 viewportMaxRegion = ImGui::GetWindowContentRegionMax();
        ImVec2 viewportOffset = ImGui::GetWindowPos();

        viewportBounds.left = ImGui::GetCursorPosX() + viewportOffset.x;
        viewportBounds.top = ImGui::GetCursorPosY() + viewportOffset.y;
        viewportBounds.right = viewportMaxRegion.x + viewportOffset.x;
        viewportBounds.bottom = viewportMaxRegion.y + viewportOffset.y;

        auto [mouseX, mouseY] = ImGui::GetMousePos();
        mouseX -= viewportBounds.left;
        mouseY -= viewportBounds.top;
        mouseY = viewportBounds.bottom - viewportBounds.top - mouseY;

        inputManager.SetMousePosition(glm::vec2(mouseX, mouseY));

        glm::vec2 viewportSize = { viewportBounds.right - viewportBounds.left, viewportBounds.bottom - viewportBounds.top };

        RenderSystem* renderSystem = nullptr;

        if(!editor.isPlaying && editor.openProject->HasEditorSystem<RenderSystem>())
            renderSystem = editor.openProject->GetEditorSystem<RenderSystem>();
       
        if(editor.isPlaying && editor.GetSceneManager()->GetActiveScene()->HasSystem<RenderSystem>())
            renderSystem = &editor.GetSceneManager()->GetActiveScene()->GetSystem<RenderSystem>();
        
        if (renderSystem)
        {
            renderSystem->SetRenderToScreen(false);

            if (viewportSize.x > 0 && viewportSize.y > 0)
                renderSystem->ResizeFramebuffer(viewportSize.x, viewportSize.y);

            ImGui::Image((ImTextureID)renderSystem->GetRenderTarget().GetRenderId(), ImVec2{ viewportSize.x, viewportSize.y }, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });

            EntityId e = renderSystem->GetMouseSelectedEntity();

            if (editor.activeScene->GetRegistry().valid(e))
            {
                auto& selection = editor.activeScene->GetRegistry().get<MouseSelectionComponent>(e);

                if (selection.status == SelectionStatus::CLICKED && !ImGuizmo::IsUsing())
                {
                    editor.selectedItem.type = SelectedItemType::ENTITY;
                    editor.selectedItem.entity = Entity(e, editor.activeScene);
                }
            }
        }
        else
        {
            ImGui::Image((ImTextureID)0, ImVec2{ viewportSize.x, viewportSize.y }, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });
        }

        if (ImGui::BeginDragDropTarget())
        {
            if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("FILE_BROWSER_SCENE"))
            {
                std::string path = (const char*)payload->Data;

                Scene* scene = new Scene();
                scene->Load(path);

                editor.GetResourceManager()->RegisterAsset(scene, path);

                editor.SwitchActiveScene(scene);
            }

            if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("FILE_BROWSER_PREFAB"))
            {
                std::string path = (const char*)payload->Data;

                Prefab p;
                p.Load(path);

                editor.selectedItem.type = SelectedItemType::ENTITY;
                editor.selectedItem.entity = editor.activeScene->InstantiatePrefab(p);
            }

            ImGui::EndDragDropTarget();
        }

        DrawTransformGuizmos();
        
        ImGui::End();
	}

	void ViewportPanel::Destroy()
	{
        editor.GetInputManager()->ListenToCursor(true);
	}

    void ViewportPanel::ProcessInput()
    {        
        InputManager& inputManager = *editor.GetInputManager();

        if (inputManager.GetKeyPressed(GET_KEYCODE(Q)))
            guizmoOperation = -1;

        if (inputManager.GetKeyPressed(GET_KEYCODE(W)) && !inputManager.GetMouseButton(MouseButton::RIGHT))
            guizmoOperation = ImGuizmo::OPERATION::TRANSLATE;

        if (inputManager.GetKeyPressed(GET_KEYCODE(E)))
            guizmoOperation = ImGuizmo::OPERATION::ROTATE;

        if (inputManager.GetKeyPressed(GET_KEYCODE(R)))
            guizmoOperation = ImGuizmo::OPERATION::SCALE;

        if (inputManager.GetKeyPressed(GET_KEYCODE(TAB)))
            local = !local;

        if (inputManager.GetKeyDown(GET_KEYCODE(LEFT_CONTROL)) && inputManager.GetKeyPressed(GET_KEYCODE(Z)))
            if (auto action = editor.actions.Pop())
            {
                action->Undo();
                delete action;
            }
        
    }
    
    void ViewportPanel::DrawTransformGuizmos()
    {
        if (editor.isPlaying || guizmoOperation == -1) return;

        auto cameras = editor.activeScene->CreateComponentView<CameraComponent>();

        if (cameras.empty()) return;

        Entity camera = { cameras.front(), editor.activeScene };

        ImGuizmo::SetOrthographic(false);
        ImGuizmo::SetDrawlist();

        ImGuizmo::SetRect(viewportBounds.left, viewportBounds.top, viewportBounds.right - viewportBounds.left, viewportBounds.bottom - viewportBounds.top);

        TransformComponent& cameraTransform = camera.GetComponent<TransformComponent>();
        CameraComponent& cameraComponent = camera.GetComponent<CameraComponent>();
        glm::mat4 cameraProjection;
        glm::mat4 cameraView;

        SelectedItem& selectedItem = editor.selectedItem;

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

                (*selectedItem.boneData.transforms)[selectedItem.boneData.id] = glm::inverse(parentTransformWorldSpace) * glm::inverse(selectedItem.boneData.owningEntity.GetGlobalTransformMatrix()) * t.GetTransformMatrix();
            }
        }
    }
}