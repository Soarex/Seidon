#include <Seidon.h>
#include <EntryPoint.h>
#include <glm/glm.hpp>

#include <Imgui/imgui_internal.h>

#include "HierarchyPanel.h"

using namespace Seidon;

class Game : public Application
{
public:
    Entity light;

    std::vector<Entity> entities;

    Game()
	{
		Window::SetName("Seidon Game");
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

            entities.push_back(e);
            i++;
        }
        

        importData = importer.Import("Assets/buildings/Assets.fbx");
        ResourceManager::CreateFromImportData(importData);

        light = EntityManager::CreateEntity("Light");
        light.AddComponent<DirectionalLightComponent>(glm::vec3(1.0f, 1.0f, 1.0f), 1.0f);
        light.GetComponent<TransformComponent>().rotation = glm::vec3(1.0f, 4.0f, 1.0f);
        entities.push_back(light);

        Entity camera = EntityManager::CreateEntity("Camera");
        camera.AddComponent<CameraComponent>().farPlane = 300;
        camera.EditComponent<TransformComponent>([](TransformComponent& t)
            {
                t.position = { 0, 10, 50 };
                t.rotation = { 0, 0, 0 };
            });
        entities.push_back(camera);

        HdrCubemap* cubemap = new HdrCubemap();
        cubemap->LoadFromEquirectangularMap("Assets/Cubemap.hdr");

        Entity cubemapEntity = EntityManager::CreateEntity("Cubemap");
        cubemapEntity.AddComponent<CubemapComponent>(cubemap);
        entities.push_back(cubemapEntity);

        SystemsManager::AddSystem<FlyingCameraControlSystem>(10, 10).SetRotationEnabled(false);
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

        ImGui::Begin("Hierarchy");
        static int selected = -1;
        for (int i = 0; i < entities.size(); i++)
        {
            if (ImGui::Selectable(entities[i].GetComponent<NameComponent>().name.c_str(), selected == i))
                selected = i;
        }
        ImGui::End();

        ImGui::Begin("Inspector");
        if (selected != -1)
        {
            Entity& entity = entities[selected];
            
            if (ImGui::CollapsingHeader("Transform Component", ImGuiTreeNodeFlags_DefaultOpen))
            {
                TransformComponent& transform = entity.GetComponent<TransformComponent>();
                DrawVec3Control("Position", transform.position, 0.0f);

                glm::vec3 rotation = glm::degrees(transform.rotation);
                DrawVec3Control("Rotation", rotation, 0.0f);
                transform.rotation = glm::radians(rotation);

                DrawVec3Control("Scale", transform.scale, 1.0f);
            }

            if (entity.HasComponent<CameraComponent>() && ImGui::CollapsingHeader("Camera Component", ImGuiTreeNodeFlags_DefaultOpen))
            {
                CameraComponent& camera = entity.GetComponent<CameraComponent>();

                ImGui::Columns(2);
                ImGui::SetColumnWidth(0, 100);

                ImGui::Text("Fov");
                ImGui::NextColumn();
                ImGui::DragFloat("##Fov", &camera.fov);
                ImGui::NextColumn();

                ImGui::Text("Near Plane");
                ImGui::NextColumn();
                ImGui::DragFloat("##Near Plane", &camera.nearPlane);
                ImGui::NextColumn();

                ImGui::Text("Far Plane");
                ImGui::NextColumn();
                ImGui::DragFloat("##Far Plane", &camera.farPlane);
                ImGui::NextColumn();

                ImGui::Text("Exposure");
                ImGui::NextColumn();
                ImGui::DragFloat("##Exposure", &camera.exposure);
                ImGui::NextColumn();

                ImGui::Columns(1);
            }

            if (entity.HasComponent<DirectionalLightComponent>() && ImGui::CollapsingHeader("Directional Light Component", ImGuiTreeNodeFlags_DefaultOpen))
            {
                DirectionalLightComponent& light = entity.GetComponent<DirectionalLightComponent>();

                ImGui::Columns(2);
                ImGui::SetColumnWidth(0, 100);

                ImGui::Text("Color");
                ImGui::NextColumn();
                ImGui::ColorEdit3("##Color", (float*)&light.color);
                ImGui::NextColumn();

                ImGui::Text("Intensity: ");
                ImGui::NextColumn();
                ImGui::DragFloat("##Intensity", &light.intensity);
                ImGui::NextColumn();

                ImGui::Columns(1);
            }

            if (entity.HasComponent<RenderComponent>() && ImGui::CollapsingHeader("Render Component", ImGuiTreeNodeFlags_DefaultOpen))
            {
                RenderComponent& renderComponent = entity.GetComponent<RenderComponent>();

                ImGui::Columns(2);
                ImGui::SetColumnWidth(0, 150);

                ImGui::Text("Mesh");
                ImGui::NextColumn();

                if (ImGui::BeginCombo("##Mesh", renderComponent.mesh->name.c_str(), 0))
                {
                    static std::string& selection = renderComponent.mesh->name;

                    for (std::string& meshName : ResourceManager::GetMeshKeys())
                    {
                        const bool is_selected = (renderComponent.mesh->name == selection);
                        if (ImGui::Selectable(meshName.c_str(), is_selected))
                        {
                            renderComponent.mesh = ResourceManager::GetMesh(meshName);
                            selection = meshName;
                        }

                        if (is_selected)
                            ImGui::SetItemDefaultFocus();
                    }
                    ImGui::EndCombo();
                }
                ImGui::NextColumn();

                
                int i = 0;
                for (SubMesh* subMesh : renderComponent.mesh->subMeshes)
                {
                    ImGui::Text(std::string("Material " + std::to_string(i)).c_str());
                    ImGui::NextColumn();

                    if (ImGui::BeginCombo(std::string("##Material " + std::to_string(i)).c_str(), subMesh->material->name.c_str(), 0))
                    {
                        static int selection = -1;

                        for (std::string& materialName : ResourceManager::GetMaterialKeys())
                        {
                            if (selection == -1 && materialName == subMesh->material->name) selection = i;

                            const bool is_selected = (i == selection);
                            if (ImGui::Selectable(materialName.c_str(), is_selected))
                            {
                                subMesh->material = ResourceManager::GetMaterial(materialName);
                                selection = i;
                            }

                            if (is_selected)
                                ImGui::SetItemDefaultFocus();

                        }
                        ImGui::EndCombo();
                    }
                    ImGui::NextColumn();

                    i++;
                }

                for (SubMesh* subMesh : renderComponent.mesh->subMeshes)
                {
                    ImGui::PushID(subMesh);
                    Material* material = subMesh->material;
                    ImGui::Columns(1);
                    if (ImGui::CollapsingHeader(material->name.c_str()))
                    {
                        ImGui::Columns(2);
                        ImGui::Text("Tint");
                        ImGui::NextColumn();
                        ImGui::ColorEdit3("##Tint", (float*)&material->tint);
                        ImGui::NextColumn();

                        static char buffer[128];
                        ImGui::Text("Albedo");
                        ImGui::NextColumn();

                        if (ImGui::BeginCombo("##Albedo", material->albedo->path.c_str(), 0))
                        {
                            static std::string& selection = material->albedo->path;

                            for (std::string& textureName : ResourceManager::GetTextureKeys())
                            {
                                const bool is_selected = (material->albedo->path == selection);
                                if (ImGui::Selectable(textureName.c_str(), is_selected))
                                {
                                    material->albedo = ResourceManager::GetTexture(textureName);
                                    selection = textureName;
                                }

                                if (is_selected)
                                    ImGui::SetItemDefaultFocus();
                            }
                            ImGui::EndCombo();
                        }

                        ImGui::NextColumn();

                        ImGui::Text("Normal");
                        ImGui::NextColumn();

                        if (ImGui::BeginCombo("##Normal", material->normal->path.c_str(), 0))
                        {
                            static std::string& selection = material->normal->path;

                            for (std::string& textureName : ResourceManager::GetTextureKeys())
                            {
                                const bool is_selected = (material->normal->path == selection);
                                if (ImGui::Selectable(textureName.c_str(), is_selected))
                                {
                                    material->normal = ResourceManager::GetTexture(textureName);
                                    selection = textureName;
                                }

                                if (is_selected)
                                    ImGui::SetItemDefaultFocus();
                            }
                            ImGui::EndCombo();
                        }

                        ImGui::NextColumn();

                        ImGui::Text("Roughness");
                        ImGui::NextColumn();

                        if (ImGui::BeginCombo("##Roughness", material->roughness->path.c_str(), 0))
                        {
                            static std::string& selection = material->roughness->path;

                            for (std::string& textureName : ResourceManager::GetTextureKeys())
                            {
                                const bool is_selected = (material->roughness->path == selection);
                                if (ImGui::Selectable(textureName.c_str(), is_selected))
                                {
                                    material->roughness = ResourceManager::GetTexture(textureName);
                                    selection = textureName;
                                }

                                if (is_selected)
                                    ImGui::SetItemDefaultFocus();
                            }
                            ImGui::EndCombo();
                        }

                        ImGui::NextColumn();

                        ImGui::Text("Metallic");
                        ImGui::NextColumn();

                        if (ImGui::BeginCombo("##Metallic", material->metallic->path.c_str(), 0))
                        {
                            static std::string& selection = material->metallic->path;

                            for (std::string& textureName : ResourceManager::GetTextureKeys())
                            {
                                const bool is_selected = (material->metallic->path == selection);
                                if (ImGui::Selectable(textureName.c_str(), is_selected))
                                {
                                    material->metallic = ResourceManager::GetTexture(textureName);
                                    selection = textureName;
                                }

                                if (is_selected)
                                    ImGui::SetItemDefaultFocus();
                            }
                            ImGui::EndCombo();
                        }
                        ImGui::NextColumn();
                    }
                    ImGui::Columns(1);
                    ImGui::PopID();
                }
                    
            }
        }
        ImGui::End();

        ImGui::Begin("Stats"); 
        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
        ImGui::End();

        ImGui::End();
	}

	~Game() 
	{

	}

	static void DrawVec3Control(const std::string& label, glm::vec3& values, float resetValue = 0.0f, float columnWidth = 100.0f)
	{
		ImGuiIO& io = ImGui::GetIO();
		auto boldFont = io.Fonts->Fonts[0];

		ImGui::PushID(label.c_str());

		ImGui::Columns(2);
		ImGui::SetColumnWidth(0, columnWidth);
		ImGui::Text(label.c_str());
		ImGui::NextColumn();

		ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, 0 });

		float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
		ImVec2 buttonSize = { lineHeight + 3.0f, lineHeight };

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.9f, 0.2f, 0.2f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
		ImGui::PushFont(boldFont);
		if (ImGui::Button("X", buttonSize))
			values.x = resetValue;
		ImGui::PopFont();
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		ImGui::DragFloat("##X", &values.x, 0.1f, 0.0f, 0.0f, "%.2f");
		ImGui::PopItemWidth();
		ImGui::SameLine();

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.3f, 0.8f, 0.3f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
		ImGui::PushFont(boldFont);
		if (ImGui::Button("Y", buttonSize))
			values.y = resetValue;
		ImGui::PopFont();
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		ImGui::DragFloat("##Y", &values.y, 0.1f, 0.0f, 0.0f, "%.2f");
		ImGui::PopItemWidth();
		ImGui::SameLine();

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.2f, 0.35f, 0.9f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
		ImGui::PushFont(boldFont);
		if (ImGui::Button("Z", buttonSize))
			values.z = resetValue;
		ImGui::PopFont();
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		ImGui::DragFloat("##Z", &values.z, 0.1f, 0.0f, 0.0f, "%.2f");
		ImGui::PopItemWidth();

		ImGui::PopStyleVar();

		ImGui::Columns(1);

		ImGui::PopID();
	}

};

Application* Seidon::CreateApplication() 
{
    return new Game();
}