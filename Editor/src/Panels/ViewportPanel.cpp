#include "ViewportPanel.h"

#include "../Editor.h"

namespace Seidon
{
	void ViewportPanel::Init()
	{

	}

	void ViewportPanel::Draw()
	{
        if (!ImGui::Begin("Viewport"))
        {
            ImGui::End();
            return;
        }

        InputManager& inputManager = *editor.GetInputManager();
        SceneManager& sceneManager = *editor.GetSceneManager();
        
        inputManager.BlockInput(!ImGui::IsWindowFocused() && !ImGui::IsWindowHovered());

        if (ImGui::Button("Colliders"));
            //colliderRenderingEnabled = !colliderRenderingEnabled;

        ImGui::SameLine();

        bool isPlaying = false;
        if (!isPlaying)
        {
            ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 5));
            ImGuiStyle& style = ImGui::GetStyle();
            float size = ImGui::CalcTextSize("Start").x + style.FramePadding.x * 2.0f;

            float offset = (ImGui::GetContentRegionAvail().x - size) * 0.5;
            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + offset);
            if (ImGui::Button("Start"))
            {
                //guizmoOperation = -1;

                //runtimeScene = new Scene("Runtime Scene");
                //scene->CopyEntities(runtimeScene);
                //runtimeSystems.CopySystems(runtimeScene);

                //sceneManager->SetActiveScene(runtimeScene);
                //selectedItem.type = SelectedItemType::NONE;
                //isPlaying = true;
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
                //sceneManager->SetActiveScene(scene);
                //delete runtimeScene;
                //selectedItem.type = SelectedItemType::NONE;
                //isPlaying = false;
            }
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

        if (sceneManager.GetActiveScene()->HasSystem<RenderSystem>())
        {
            RenderSystem& renderSystem = sceneManager.GetActiveScene()->GetSystem<RenderSystem>();
            renderSystem.SetRenderToScreen(false);

            if (viewportSize.x > 0 && viewportSize.y > 0)
                renderSystem.ResizeFramebuffer(viewportSize.x, viewportSize.y);

            ImGui::Image((ImTextureID)renderSystem.GetRenderTarget().GetRenderId(), ImVec2{ viewportSize.x, viewportSize.y }, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });

            EntityId e = renderSystem.GetMouseSelectedEntity();

            if (sceneManager.GetActiveScene()->GetRegistry().valid(e))
            {
                auto& selection = sceneManager.GetActiveScene()->GetRegistry().get<MouseSelectionComponent>(e);

                if (selection.status == SelectionStatus::CLICKED/* && !ImGuizmo::IsUsing()*/)
                {
                    editor.selectedItem.type = SelectedItemType::ENTITY;
                    editor.selectedItem.entity = Entity(e, sceneManager.GetActiveScene());
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

                //runtimeSystems.Destroy();
                //scene->Destroy();

                //Scene tempScene;

                //std::ifstream in(path, std::ios::in | std::ios::binary);
                //tempScene.Load(in);

                //tempScene.CopyEntities(scene);
                //tempScene.CopySystems(&runtimeSystems);

                //scene->AddSystem<RenderSystem>().AddMainRenderPassFunction(drawColliders);
                //scene->AddSystem<EditorCameraControlSystem>();

                //selectedItem.type = SelectedItemType::NONE;
            }

            if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("FILE_BROWSER_PREFAB"))
            {
                std::string path = (const char*)payload->Data;

                //Prefab p;
                //p.Load(path);

                //selectedItem.type = SelectedItemType::ENTITY;
                //selectedItem.entity = scene->InstantiatePrefab(p);
            }

            ImGui::EndDragDropTarget();
        }

        
        ImGui::End();
	}

	void ViewportPanel::Destroy()
	{

	}

    void ViewportPanel::ProcessInput()
    {
        int guizmoOperation;
        bool local = false;
        if (!editor.isPlaying)
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

            //if (inputManager->GetKeyPressed(GET_KEYCODE(LEFT_CONTROL)) && inputManager->GetKeyPressed(GET_KEYCODE(Z)) && inputManager->GetKeyPressed(GET_KEYCODE(LEFT_SHIFT)))
            //    if (auto action = actions.Pop()) action->Do();
        }
    }
}