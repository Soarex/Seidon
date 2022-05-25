#include "HierarchyPanel.h"
#include <ostream>
namespace Seidon
{
	void HierarchyPanel::Init()
	{
	}

	void HierarchyPanel::Draw()
	{
		if (!ImGui::Begin("Hierarchy"))
		{
			ImGui::End();
			return;
		}

		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 1));
		ImGui::Dummy({ ImGui::GetWindowSize().x, 10});
		ImGui::PopStyleVar();

		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("HIERARCHY_PANEL_ENTITY"))
			{
				Entity& e = *(Entity*)payload->Data;
				e.RemoveParent();
			}

			ImGui::EndDragDropTarget();
		}

		Application::Get()->GetSceneManager()->GetActiveScene()->GetRegistry().each([&](auto entityId)
			{
				Scene* scene = Application::Get()->GetSceneManager()->GetActiveScene();
				if (!scene->IsEntityIdValid(entityId)) return;

				Entity entity(entityId, scene);

				UUID parentId = entity.GetComponent<TransformComponent>().parent;

				if (scene->IsIdValid(parentId)) return;

				DrawEntityNode(entity);

			});

		ImGui::Dummy({ ImGui::GetWindowSize().x, 25 });
		
		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("HIERARCHY_PANEL_ENTITY"))
			{
				Entity& e = *(Entity*)payload->Data;

				e.RemoveParent();
			}

			ImGui::EndDragDropTarget();
		}

		if (ImGui::BeginPopupContextWindow(0, 1, false))
		{
			if (ImGui::MenuItem("Create Empty Entity"))
			{
				Entity e = Application::Get()->GetSceneManager()->GetActiveScene()->CreateEntity();

				selectedItem.type = SelectedItemType::ENTITY;
				selectedItem.entity = e;
			}

			if (ImGui::MenuItem("Create Mesh Entity"))
			{
				Entity e = Application::Get()->GetSceneManager()->GetActiveScene()->CreateEntity();
				e.AddComponent<RenderComponent>();

				selectedItem.type = SelectedItemType::ENTITY;
				selectedItem.entity = e;
			}

			if (ImGui::MenuItem("Create Animated Mesh Entity"))
			{
				Entity e = Application::Get()->GetSceneManager()->GetActiveScene()->CreateEntity();
				e.AddComponent<SkinnedRenderComponent>();
				e.AddComponent<AnimationComponent>();

				selectedItem.type = SelectedItemType::ENTITY;
				selectedItem.entity = e;
			}

			ImGui::EndPopup();
		}


		ImGui::End();
	}

	void HierarchyPanel::DrawEntityNode(Entity& entity)
	{
		std::string& name = entity.GetComponent<NameComponent>().name;
		UUID id = entity.GetComponent<IDComponent>().ID;

		ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_FramePadding | ImGuiTreeNodeFlags_SpanAvailWidth;
		
		if (selectedItem.type == SelectedItemType::ENTITY && selectedItem.entity == entity)
			flags |= ImGuiTreeNodeFlags_Selected;

		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
		bool open = ImGui::TreeNodeEx((void*)(long long)id, flags, name.c_str());

		if (ImGui::BeginDragDropSource())
		{
			ImGui::SetDragDropPayload("HIERARCHY_PANEL_ENTITY", &entity, sizeof(Entity));
			ImGui::EndDragDropSource();
		}

		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("HIERARCHY_PANEL_ENTITY"))
			{
				Entity& e = *(Entity*)payload->Data;

				if (!e.IsAncestorOf(entity))
					e.SetParent(entity);

			}

			ImGui::EndDragDropTarget();
		}

		if (ImGui::IsItemClicked())
		{
			selectedItem.type = SelectedItemType::ENTITY;
			selectedItem.entity = entity;
		}

		ImGui::PopStyleVar();
		bool entityDeleted = false;
		if (ImGui::BeginPopupContextItem())
		{
			if (ImGui::MenuItem("Delete Entity"))
				entityDeleted = true;

			ImGui::EndPopup();
		}
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));

		if(open)
		{
			if (entity.HasComponent<SkinnedRenderComponent>())
			{
				SkinnedRenderComponent& r = entity.GetComponent<SkinnedRenderComponent>();
				DrawBoneNode(entity, r.mesh->armature, r.boneTransforms, 0);
			}

			TransformComponent& t = entity.GetComponent<TransformComponent>();

			for (UUID i : t.children)
			{
				Entity e = entity.scene->GetEntityById(i);
				DrawEntityNode(e);
			}
			ImGui::TreePop();
		}

		if (entityDeleted)
		{
			if (selectedItem.type == SelectedItemType::ENTITY && (selectedItem.entity == entity || selectedItem.entity.IsDescendantOf(entity)))
				selectedItem.type = SelectedItemType::NONE;

			if(selectedItem.type == SelectedItemType::BONE)
				selectedItem.type = SelectedItemType::NONE;

			Application::Get()->GetSceneManager()->GetActiveScene()->DestroyEntity(entity);
		}

		ImGui::PopStyleVar();
	}

	void HierarchyPanel::DrawBoneNode(Entity& owningEntity, Armature& armature, std::vector<glm::mat4>& boneTransforms, int index)
	{
		ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_FramePadding | ImGuiTreeNodeFlags_SpanAvailWidth;

		if (selectedItem.type == SelectedItemType::BONE && selectedItem.boneData.armature->bones[selectedItem.boneData.id] == armature.bones[index])
			flags |= ImGuiTreeNodeFlags_Selected;

		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
		ImGui::PushStyleColor(ImGuiCol_Text, {1.0f, 1.0f, 0.0, 0.7f});

		bool open = ImGui::TreeNodeEx(armature.bones[index].name.c_str(), flags, armature.bones[index].name.c_str());

		if (ImGui::IsItemClicked())
		{
			selectedItem.type = SelectedItemType::BONE;
			selectedItem.boneData.id = index;
			selectedItem.boneData.owningEntity = owningEntity;
			selectedItem.boneData.armature = &armature;
			selectedItem.boneData.transforms = &boneTransforms;
		}

		if (open)
		{
			for (int i = index + 1; i < armature.bones.size(); i++)
				if(armature.bones[i].parentId == index)
					DrawBoneNode(owningEntity, armature, boneTransforms, i);

			ImGui::TreePop();
		}

		ImGui::PopStyleColor();
		ImGui::PopStyleVar();
	}
}
