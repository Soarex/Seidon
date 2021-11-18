#include "InspectorPanel.h"

namespace Seidon
{
	void InspectorPanel::Init()
	{
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
        auto& name = selectedEntity.GetComponent<NameComponent>().name;

        char buffer[256];
        std::strncpy(buffer, name.c_str(), sizeof(buffer));

        ImGui::PushItemWidth(-1);
        if (ImGui::InputText("##Name", buffer, sizeof(buffer)))
            name = std::string(buffer);
        ImGui::PopItemWidth();

        const std::vector<ComponentMetaType>& components = Application::Get()->GetComponentMetaTypes();

        for (auto& metaType : components)
        {
            if (metaType.name == typeid(IDComponent).name() || metaType.name == typeid(NameComponent).name()) continue;

            bool hasComponent = metaType.Has(selectedEntity);
            if (hasComponent && ImGui::CollapsingHeader(metaType.name.c_str() + 7, ImGuiTreeNodeFlags_DefaultOpen))
            {
                char* component = (char*)metaType.Get(selectedEntity);
                
                for (const MemberData& member : metaType.members)
                {
                    if (member.type == Types::FLOAT)
                        DrawFloatControl(member.name.c_str(), *(float*)(component + member.offset));

                    if (member.type == Types::VECTOR3)
                        DrawVec3Control(member.name.c_str(),*(glm::vec3*)(component + member.offset), 0.0f);

                    if (member.type == Types::VECTOR3_ANGLES)
                    {
                        glm::vec3 temp = glm::degrees(*(glm::vec3*)(component + member.offset));
                        DrawVec3Control(member.name.c_str(), temp, 0.0f);
                        *(glm::vec3*)(component + member.offset) = glm::radians(temp);
                    }

                    if (member.type == Types::VECTOR3_COLOR)
                        DrawColorControl(member.name.c_str(), *(glm::vec3*)(component + member.offset));

                    if (member.type == Types::TEXTURE)
                    {
                        Texture* t = *(Texture**)(component + member.offset);
                        DrawTextureControl(member.name.c_str(), t);
                    }

                    if (member.type == Types::MESH)
                    {
                        Mesh** m = (Mesh**)(component + member.offset);
                        DrawMeshControl(member.name.c_str(), m);
                    }

                    if (member.type == Types::MATERIAL_VECTOR)
                    {
                        std::vector<Material*>* v = (std::vector<Material*>*)(component + member.offset);

                        static bool open;
                        static int selected;

                        for (int i = 0; i < v->size(); i++)
                            if (DrawMaterialControl(member.name.c_str(), &v->at(i)))
                            {
                                if (selected == i && open)
                                    open = false;
                                else
                                {
                                    selected = i;
                                    open = true;
                                }
                            }

                        if (open && selected < v->size())
                            DrawMaterialEditor("Material Editor", v->at(selected), &open);
                    }

                    if (member.type == Types::CUBEMAP)
                    {
                        HdrCubemap** c = (HdrCubemap**)(component + member.offset);
                        DrawCubemapControl(member.name.c_str(), c);
                    }

                }
            }

        }

        ImGui::Spacing();
        ImGui::Spacing();

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