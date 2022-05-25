#include "InspectorPanel.h"
#include "../Editor.h"

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

		if (selectedItem.type == SelectedItemType::ENTITY)
            DrawEntity(selectedItem.entity);

        if (selectedItem.type == SelectedItemType::BONE)
            DrawBone(selectedItem.boneData);

        if (selectedItem.type == SelectedItemType::MATERIAL)
            DrawMaterial(selectedItem.material);
		
		ImGui::End();
	}

    void InspectorPanel::DrawMaterial(Material* m)
    {
        DrawMaterialEditor("Material Editor", m);
    }

    void InspectorPanel::DrawBone(BoneSelectionData& boneData)
    {
        BoneData& bone = boneData.armature->bones[boneData.id];

        char buffer[256];
        std::strncpy(buffer,bone.name.c_str(), sizeof(buffer));
        ImGui::PushItemWidth(-1);
        ImGui::InputText("##Name", buffer, sizeof(buffer), ImGuiInputTextFlags_ReadOnly);
        ImGui::PopItemWidth();
        
        TransformComponent t;
        t.SetFromMatrix((*boneData.transforms)[boneData.id]);

        MetaType type = Application::Get()->GetComponentMetaType<TransformComponent>();

        if(ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen))
            DrawMetaType(&t, type);

        (*boneData.transforms)[boneData.id] = t.GetTransformMatrix();
    }

    void InspectorPanel::DrawEntity(Entity e)
    {
        auto& name = e.GetComponent<NameComponent>().name;

        char buffer[256];
        std::strncpy(buffer, name.c_str(), sizeof(buffer));

        ImGui::PushItemWidth(-1);
        if (ImGui::InputText("##Name", buffer, sizeof(buffer)))
            name = std::string(buffer);
        ImGui::PopItemWidth();

        std::vector<ComponentMetaType> components = Application::Get()->GetComponentMetaTypes();

        for (auto& metaType : components)
        {
            if (metaType.name == typeid(IDComponent).name() || metaType.name == typeid(NameComponent).name() 
                || metaType.name == typeid(MouseSelectionComponent).name()) continue;

            if (!metaType.Has(e)) continue;
            
            bool open = ImGui::CollapsingHeader(metaType.name.c_str() + 7, ImGuiTreeNodeFlags_DefaultOpen);
            
            if (metaType.name != typeid(TransformComponent).name() && ImGui::BeginPopupContextItem())
            {
                if (ImGui::MenuItem("Remove Component"))
                    metaType.Remove(e);

                ImGui::EndPopup();
                continue;
            }

            if (metaType.Has(e) && open)
            {
                ChangeData change = DrawMetaType(metaType.Get(e), metaType);

                if (change.status == ChangeStatus::CHANGED)
                {
                    EditorComponentAction* action = new EditorComponentAction();
                    action->modifiedEntity = e;
                    action->modifiedMetaType = metaType;
                    action->modifiedMemberName = change.modifiedMember.name;

                    memcpy(action->oldValue, change.oldValue, sizeof(action->oldValue));
                    memcpy(action->newValue, change.newValue, sizeof(action->newValue));

                    Editor& editor = *(Editor*)Application::Get();
                    editor.OnEditorAction(action);

                    //glm::vec3 oldValue = *(glm::vec3*)action->oldValue;
                    //glm::vec3 newValue = *(glm::vec3*)action->newValue;

                    //std::cout << metaType.name << ": " << change.modifiedMember.name << std::endl;
                    //std::cout << oldValue.x << " ," << oldValue.y << " ," << oldValue.z << std::endl;
                    //std::cout << newValue.x << " ," << newValue.y << " ," << newValue.z << std::endl << std::endl;
                }
            }

        }

        ImGui::Separator();

        ImGuiStyle& style = ImGui::GetStyle();

        float size = ImGui::CalcTextSize("Add Component").x  + style.FramePadding.x * 2.0f + ImGui::CalcTextSize("Save as prefab").x + style.FramePadding.x * 2.0f;
        
        float offset = (ImGui::GetContentRegionAvail().x - size) * 0.5;
        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + offset);

        if (ImGui::Button("Add Component"))
            ImGui::OpenPopup("AddComponent");

        ImGui::SameLine();

        if (ImGui::Button("Save as prefab"))
            ImGui::OpenPopup("Save as prefab");

        if (ImGui::BeginPopup("AddComponent"))
        {
            for (auto& metaType : components)
            {
                bool hasComponent = metaType.Has(e);

                if (!hasComponent && ImGui::MenuItem(metaType.name.c_str() + 7))
                    metaType.Add(e);
            }

            ImGui::EndPopup();
        }

        if (ImGui::BeginPopupModal("Save as prefab", 0, ImGuiWindowFlags_AlwaysAutoResize))
        {
            ImGui::AlignTextToFramePadding();
            ImGui::Text("Name: ");
            ImGui::SameLine();

            static char buffer[512];
            ImGui::InputText("##NewName", buffer, 512);

            ImGuiStyle& style = ImGui::GetStyle();
            float size = ImGui::CalcTextSize("Save").x  + style.FramePadding.x * 2.0f + ImGui::CalcTextSize("Close").x + style.FramePadding.x * 2.0f;

            float offset = (ImGui::GetContentRegionAvail().x - size) * 0.5;
            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + offset);
            if (ImGui::Button("Save"))
            {
                Prefab p;
                p.MakeFromEntity(e);

                p.Save("Assets\\" + std::string(buffer) + ".sdpref");
                ImGui::CloseCurrentPopup();
            }

            ImGui::SameLine();

            if (ImGui::Button("Close"))
                ImGui::CloseCurrentPopup();

            ImGui::EndPopup();
        }
    }
}