#include <Seidon.h>
#include <EntryPoint.h>
#include <glm/glm.hpp>

#include "Panels/InspectorPanel.h"
#include "Panels/HierarchyPanel.h"
#include "Panels/AssetBrowserPanel.h"

using namespace Seidon;

class Editor : public Application
{
public:
    Entity selectedEntity;

    HierarchyPanel hierarchyPanel;
    InspectorPanel inspectorPanel;
    AssetBrowserPanel assetBrowserPanel;

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

        Entity camera = EntityManager::CreateEntity("Camera");
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

        SystemsManager::AddSystem<FlyingCameraControlSystem>(10, 0.5).SetRotationEnabled(false);

        selectedEntity = { entt::null, &EntityManager::registry };
        hierarchyPanel.AddSelectionCallback([&](Entity& entity)
            {
                selectedEntity = entity;
            });
	}

	void Run()
	{
        RenderSystem& renderSystem = SystemsManager::GetSystem<RenderSystem>();
        FlyingCameraControlSystem& cameraControlSystem = SystemsManager::GetSystem<FlyingCameraControlSystem>();

        if (InputManager::GetKeyPressed(GET_KEYCODE(BACKSLASH)))
            Window::ToggleFullscreen();

        if (InputManager::GetKeyPressed(GET_KEYCODE(ESCAPE)))
        {
            Window::ToggleMouseCursor();
            cameraControlSystem.ToggleRotation();
        }

        static bool dockspaceOpen = true;
        static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

        ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
        ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->Pos);
        ImGui::SetNextWindowSize(viewport->Size);
        ImGui::SetNextWindowViewport(viewport->ID);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
        window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

        if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
            window_flags |= ImGuiWindowFlags_NoBackground;

        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
        ImGui::Begin("DockSpace Window", &dockspaceOpen, window_flags);
        ImGui::PopStyleVar(3);

        // DockSpace
        ImGuiIO& io = ImGui::GetIO();
        ImGuiStyle& style = ImGui::GetStyle();
        float minWinSizeX = style.WindowMinSize.x;
        style.WindowMinSize.x = 370.0f;
        if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
        {
            ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
            ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
        }

        ImGui::Begin("Viewport");
        ImVec2 viewportMinRegion = ImGui::GetWindowContentRegionMin();
        ImVec2 viewportMaxRegion = ImGui::GetWindowContentRegionMax();
        ImVec2 viewportOffset = ImGui::GetWindowPos();

        ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
        renderSystem.ResizeFramebuffer(viewportPanelSize.x, viewportPanelSize.y);

        ImGui::Image((ImTextureID)renderSystem.GetRenderTarget().GetId(), ImVec2{ viewportPanelSize.x, viewportPanelSize.y }, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });
        ImGui::End();

        hierarchyPanel.Draw();

        inspectorPanel.SetSelectedEntity(selectedEntity);
        inspectorPanel.Draw();
       
        assetBrowserPanel.Draw();

        ImGui::Begin("Stats"); 
        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
        ImGui::End();

        ImGui::End();
	}

	~Editor()
	{

	}
};

Application* Seidon::CreateApplication() 
{
    return new Editor();
}