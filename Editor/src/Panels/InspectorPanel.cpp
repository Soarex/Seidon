#include "InspectorPanel.h"

namespace Seidon
{
	void InspectorPanel::Init()
	{
	}

	void InspectorPanel::Draw()
	{
        if (!ImGui::Begin("Inspector"))
        {
            ImGui::End();
            return;
        }

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
        auto& name = selectedEntity.GetComponent<NameComponent>().name;

        char buffer[256];
        std::strncpy(buffer, name.c_str(), sizeof(buffer));

        ImGui::PushItemWidth(-1);
        if (ImGui::InputText("##Name", buffer, sizeof(buffer)))
            name = std::string(buffer);
        ImGui::PopItemWidth();

        std::vector<ComponentMetaType> components = Application::Get()->GetComponentMetaTypes();

        for (auto& metaType : components)
        {
            if (metaType.name == typeid(IDComponent).name() || metaType.name == typeid(NameComponent).name()) continue;
            if (!metaType.Has(selectedEntity)) continue;
            
            bool open = ImGui::CollapsingHeader(metaType.name.c_str() + 7, ImGuiTreeNodeFlags_DefaultOpen);
            

            if (metaType.name != typeid(TransformComponent).name() && ImGui::BeginPopupContextItem())
            {
                if (ImGui::MenuItem("Remove Component"))
                    metaType.Remove(selectedEntity);

                ImGui::EndPopup();
                continue;
            }

            if (metaType.Has(selectedEntity) && open)
                DrawReflectedMembers(metaType.Get(selectedEntity), metaType.members);

        }

        ImGui::Separator();

        ImGuiStyle& style = ImGui::GetStyle();

        float size = ImGui::CalcTextSize("Add Component").x + style.FramePadding.x * 2.0f;
        
        float offset = (ImGui::GetContentRegionAvail().x - size) * 0.5;
        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + offset);

        if (ImGui::Button("Add Component"))
            ImGui::OpenPopup("AddComponent");

        if (ImGui::BeginPopup("AddComponent"))
        {
            for (auto& metaType : components)
            {
                bool hasComponent = metaType.Has(selectedEntity);

                if (!hasComponent && ImGui::MenuItem(metaType.name.c_str() + 7))
                    metaType.Add(selectedEntity);
            }

            ImGui::EndPopup();
        }
    }
}