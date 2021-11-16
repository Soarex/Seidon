#include "HierarchyPanel.h"

namespace Seidon
{
	void HierarchyPanel::Init()
	{
		selectedEntity = { entt::null, nullptr };
	}

	void HierarchyPanel::Draw()
	{
		ImGui::Begin("Hierarchy");

		Application::Get()->GetSceneManager()->GetActiveScene()->GetRegistry().each([&](auto entityID)
			{
				Entity entity(entityID, &Application::Get()->GetSceneManager()->GetActiveScene()->GetRegistry());
				DrawEntityNode(entity);
			});

		if (ImGui::BeginPopupContextWindow(0, 1, false))
		{
			if (ImGui::MenuItem("Create Empty Entity"))
			{
				Entity entity = Application::Get()->GetSceneManager()->GetActiveScene()->CreateEntity();

				selectedEntity = entity;

				for (auto& callback : onEntitySelectionCallbacks)
					callback(entity);
			}

			ImGui::EndPopup();
		}


		ImGui::End();
	}

	void HierarchyPanel::DrawEntityNode(Entity& entity)
	{
		std::string& name = entity.GetComponent<NameComponent>().name;

		ImGuiTreeNodeFlags flags = ((selectedEntity == entity) ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_OpenOnArrow;
		flags |= ImGuiTreeNodeFlags_SpanAvailWidth;

		if (ImGui::TreeNodeEx((void*)entity.ID, flags, name.c_str()))
			ImGui::TreePop();

		if (ImGui::IsItemClicked())
		{
			selectedEntity = entity;

			for (auto& callback : onEntitySelectionCallbacks)
				callback(entity);
		}

		bool entityDeleted = false;
		if (ImGui::BeginPopupContextItem())
		{
			if (ImGui::MenuItem("Delete Entity"))
				entityDeleted = true;

			ImGui::EndPopup();
		}


		if (entityDeleted)
		{
			Application::Get()->GetSceneManager()->GetActiveScene()->DestroyEntity(entity);
			if (selectedEntity == entity)
			{
				selectedEntity = { entt::null, nullptr };
				for (auto& callback : onEntitySelectionCallbacks)
					callback(selectedEntity);
			}
		}
	}

	void HierarchyPanel::AddSelectionCallback(const std::function<void(Entity&)>& callback)
	{
		onEntitySelectionCallbacks.push_back(callback);
	}

	void HierarchyPanel::SetSelectedEntity(Entity& entity)
	{
		selectedEntity = entity;
	}
}
