#include "InspectorPanel.h"

namespace Seidon
{
	void InspectorPanel::Init()
	{
		AddDrawFunction<NameComponent>([](Entity& entity)
			{
				auto& name = entity.GetComponent<NameComponent>().name;

				char buffer[256];
				std::strncpy(buffer, name.c_str(), sizeof(buffer));

                ImGui::PushItemWidth(-1);
				if (ImGui::InputText("##Name", buffer, sizeof(buffer)))
					name = std::string(buffer);
                ImGui::PopItemWidth();
			});

		AddDrawFunction<TransformComponent>([](Entity& entity)
			{
				TransformComponent& transform = entity.GetComponent<TransformComponent>();
				DrawVec3Control("Position", transform.position, 0.0f);

				glm::vec3 rotation = glm::degrees(transform.rotation);
				DrawVec3Control("Rotation", rotation, 0.0f);
				transform.rotation = glm::radians(rotation);

				DrawVec3Control("Scale", transform.scale, 1.0f);
			});

		AddDrawFunction<CameraComponent>([](Entity& entity)
			{
				CameraComponent& camera = entity.GetComponent<CameraComponent>();

				DrawFloatControl("Fov", camera.fov);
				DrawFloatControl("Near Plane", camera.nearPlane);
				DrawFloatControl("Far Plane", camera.farPlane);
				DrawFloatControl("Exposure", camera.exposure);
			});

		AddDrawFunction<DirectionalLightComponent>([](Entity& entity)
			{
				DirectionalLightComponent& light = entity.GetComponent<DirectionalLightComponent>();

				ImGui::Columns(2);
				ImGui::SetColumnWidth(0, 100);

				ImGui::Text("Color");
				ImGui::NextColumn();
				ImGui::ColorEdit3("##Color", (float*)&light.color);
				ImGui::NextColumn();

				DrawFloatControl("Intensity", light.intensity);

				ImGui::Columns(1);
			});

        AddDrawFunction<RigidbodyComponent>([](Entity& entity)
            {
                RigidbodyComponent& rigidbody = entity.GetComponent<RigidbodyComponent>();

                DrawFloatControl("Mass", rigidbody.mass);
            });

        AddDrawFunction<CubeColliderComponent>([](Entity& entity)
            {
                CubeColliderComponent& collider = entity.GetComponent<CubeColliderComponent>();

                DrawVec3Control("Half Extents", collider.halfExtents);
            });

		AddDrawFunction<RenderComponent>([](Entity& entity)
			{
                ResourceManager* resourceManager = Application::Get()->GetResourceManager();
                RenderComponent& renderComponent = entity.GetComponent<RenderComponent>();

                ImGui::Columns(2);
                ImGui::SetColumnWidth(0, 150);

                ImGui::Text("Mesh");
                ImGui::NextColumn();

                if (ImGui::BeginCombo("##Mesh", renderComponent.mesh->name.c_str(), 0))
                {
                    static std::string& selection = renderComponent.mesh->name;

                    for (std::string& meshName : resourceManager->GetMeshKeys())
                    {
                        const bool is_selected = (renderComponent.mesh->name == selection);
                        if (ImGui::Selectable(meshName.c_str(), is_selected))
                        {
                            renderComponent.mesh = resourceManager->GetMesh(meshName);
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

                        for (std::string& materialName : resourceManager->GetMaterialKeys())
                        {
                            if (selection == -1 && materialName == subMesh->material->name) selection = i;

                            const bool is_selected = (i == selection);
                            if (ImGui::Selectable(materialName.c_str(), is_selected))
                            {
                                subMesh->material = resourceManager->GetMaterial(materialName);
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
                        ImGui::Columns(1);

                        static char buffer[128];
                        DrawTextureControl("Albedo", material->albedo);
                        DrawTextureControl("Normal", material->normal);
                        DrawTextureControl("Roughness", material->roughness);
                        DrawTextureControl("Metallic", material->metallic);
                        DrawTextureControl("Ambient Occlusion", material->ao);

                    }
                    ImGui::PopID();
                }
			});
	}

	void InspectorPanel::Draw()
	{
        ImGui::Begin("Inspector");

		if (selectedEntity.ID == entt::null)
		{
			ImGui::End();
			return;
		}

		DrawComponents();
		ImGui::End();
	}

	void InspectorPanel::SetSelectedEntity(Entity& entity)
	{
		selectedEntity = entity;
	}

    void InspectorPanel::DrawComponents()
    {
		DrawComponent<NameComponent>(selectedEntity);

        if(ImGui::CollapsingHeader("Transform Component", ImGuiTreeNodeFlags_DefaultOpen))
		    DrawComponent<TransformComponent>(selectedEntity);
		
		if (selectedEntity.HasComponent<CameraComponent>() && ImGui::CollapsingHeader("Camera Component", ImGuiTreeNodeFlags_DefaultOpen))
			DrawComponent<CameraComponent>(selectedEntity);

		if(selectedEntity.HasComponent<DirectionalLightComponent>() && ImGui::CollapsingHeader("Directional Light Component", ImGuiTreeNodeFlags_DefaultOpen))
			DrawComponent<DirectionalLightComponent>(selectedEntity);

		if (selectedEntity.HasComponent<RenderComponent>() && ImGui::CollapsingHeader("Render Component", ImGuiTreeNodeFlags_DefaultOpen))
			DrawComponent<RenderComponent>(selectedEntity);

        if (selectedEntity.HasComponent<RigidbodyComponent>() && ImGui::CollapsingHeader("Rigidbody Component", ImGuiTreeNodeFlags_DefaultOpen))
            DrawComponent<RigidbodyComponent>(selectedEntity);

        if (selectedEntity.HasComponent<CubeColliderComponent>() && ImGui::CollapsingHeader("Cube Collider Component", ImGuiTreeNodeFlags_DefaultOpen))
            DrawComponent<CubeColliderComponent>(selectedEntity);

        
        ImGui::Spacing();
        ImGui::Spacing();

        if (ImGui::Button("Add Component"))
            ImGui::OpenPopup("AddComponent");

        if (ImGui::BeginPopup("AddComponent"))
        {
            for (auto metaType : entt::resolve())
            {
                std::string name(metaType.info().name());
                entt::meta_func hasFunction = metaType.func(entt::hashed_string("HasComponent"));
                entt::registry& r = *selectedEntity.registry;
                
                bool hasComponent = hasFunction.invoke({}, entt::forward_as_meta<entt::registry&>(r), selectedEntity.ID).cast<bool>();

                if (!hasComponent && ImGui::MenuItem(name.c_str()))
                {
                    entt::meta_func addFunction = metaType.func(entt::hashed_string("AddComponent"));
                    addFunction.invoke({}, entt::forward_as_meta<entt::registry&>(r), selectedEntity.ID);
                }
            }
            ImGui::EndPopup();
        }
    }
}