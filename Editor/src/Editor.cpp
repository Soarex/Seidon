#include <Seidon.h>
#include <EntryPoint.h>
#include <glm/glm.hpp>
#include <ImGuizmo/ImGuizmo.h>

#include "Panels/InspectorPanel.h"
#include "Panels/HierarchyPanel.h"
#include "Panels/AssetBrowserPanel.h"
#include "Dockspace.h"

#include "EditorCameraControlSystem.h"

using namespace Seidon;

class Editor : public Application
{
public:
    Entity selectedEntity;

    HierarchyPanel hierarchyPanel;
    InspectorPanel inspectorPanel;
    AssetBrowserPanel assetBrowserPanel;
    Dockspace dockspace;

    Entity camera;
    Editor()
	{
		Window::SetName("Seidon Editor");
        Window::SetSize(1280, 720);

        ModelImporter importer;
        ModelImportData importData = importer.Import("Assets/untitled.fbx");
        std::vector<Mesh*> meshes = ResourceManager::CreateFromImportData(importData);

        int i = 0;
        for (Mesh* mesh : meshes)
        {
            Entity e = EntityManager::CreateEntity(mesh->name);
            e.GetComponent<TransformComponent>().SetFromMatrix(importData.localTransforms[i]);
            e.AddComponent<RenderComponent>(mesh);

            i++;
        }
        

        importData = importer.Import("Assets/buildings/Assets.fbx");
        ResourceManager::CreateFromImportData(importData);

        Entity light = EntityManager::CreateEntity("Light");
        light.AddComponent<DirectionalLightComponent>(glm::vec3(1.0f, 1.0f, 1.0f), 1.0f);
        light.GetComponent<TransformComponent>().rotation = glm::vec3(1.0f, 4.0f, 1.0f);

        camera = EntityManager::CreateEntity("Camera");
        camera.AddComponent<CameraComponent>().farPlane = 300;
        camera.EditComponent<TransformComponent>([](TransformComponent& t)
            {
                t.position = { 0, 10, 50 };
                t.rotation = { 0, 0, 0 };
            });

        HdrCubemap* cubemap = new HdrCubemap();
        cubemap->LoadFromEquirectangularMap("Assets/Cubemap.hdr");

        Entity cubemapEntity = EntityManager::CreateEntity("Cubemap");
        cubemapEntity.AddComponent<CubemapComponent>(cubemap);

        SystemsManager::AddSystem<EditorCameraControlSystem>(10, 0.5).SetRotationEnabled(true);

        selectedEntity = { entt::null, &EntityManager::registry };
        hierarchyPanel.AddSelectionCallback([&](Entity& entity)
            {
                selectedEntity = entity;
            });

        ImGuiIO& io = ImGui::GetIO();
        io.Fonts->AddFontFromFileTTF("Assets/Roboto-Regular.ttf", 18);
	}

    int guizmoOperation = -1;
    bool local = false;
	void Run()
	{
        RenderSystem& renderSystem = SystemsManager::GetSystem<RenderSystem>();
        EditorCameraControlSystem& cameraControlSystem = SystemsManager::GetSystem<EditorCameraControlSystem>();

        if (InputManager::GetKeyPressed(GET_KEYCODE(BACKSLASH)))
            Window::ToggleFullscreen();

        if (InputManager::GetKeyPressed(GET_KEYCODE(Q)))
            guizmoOperation = -1;

        if (InputManager::GetKeyPressed(GET_KEYCODE(W)) && !InputManager::GetMouseButton(MouseButton::RIGHT))
            guizmoOperation = ImGuizmo::OPERATION::TRANSLATE;

        if (InputManager::GetKeyPressed(GET_KEYCODE(E)))
            guizmoOperation = ImGuizmo::OPERATION::ROTATE;

        if (InputManager::GetKeyPressed(GET_KEYCODE(R)))
            guizmoOperation = ImGuizmo::OPERATION::SCALE;

        if (InputManager::GetKeyPressed(GET_KEYCODE(TAB)))
            local = !local;

        dockspace.Begin();

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

	~Editor()
	{

	}
};

Application* Seidon::CreateApplication() 
{
    return new Editor();
}