#include "HierarchyPanel.h"
#include <ostream>
namespace Seidon
{
	void HierarchyPanel::Init()
	{
		selectedEntity = { entt::null, nullptr };
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
				Entity entity = Application::Get()->GetSceneManager()->GetActiveScene()->CreateEntity();

				selectedEntity = entity;

				for (auto& callback : onEntitySelectionCallbacks)
					callback(entity);
			}

			if (ImGui::MenuItem("Create Mesh Entity"))
			{
				Entity entity = Application::Get()->GetSceneManager()->GetActiveScene()->CreateEntity();
				entity.AddComponent<RenderComponent>();

				selectedEntity = entity;

				for (auto& callback : onEntitySelectionCallbacks)
					callback(entity);
			}

			if (ImGui::MenuItem("Create Animated Mesh Entity"))
			{
				Entity entity = Application::Get()->GetSceneManager()->GetActiveScene()->CreateEntity();
				entity.AddComponent<SkinnedRenderComponent>();
				entity.AddComponent<AnimationComponent>();

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
		UUID id = entity.GetComponent<IDComponent>().ID;

		ImGuiTreeNodeFlags flags = ((selectedEntity == entity) ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_FramePadding;
		flags |= ImGuiTreeNodeFlags_SpanAvailWidth;

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
				{
					e.SetParent(entity);

					//glm::mat4 localTransformToParent = glm::inverse(entity.GetComponent<TransformComponent>().GetTransformMatrix()) * e.GetComponent<TransformComponent>().GetTransformMatrix();
					//e.GetComponent<TransformComponent>().SetFromMatrix(localTransformToParent);
				}
			}

			ImGui::EndDragDropTarget();
		}

		if (ImGui::IsItemClicked())
		{
			selectedEntity = entity;

			for (auto& callback : onEntitySelectionCallbacks)
				callback(entity);
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
			if (selectedEntity == entity || selectedEntity.IsDescendantOf(entity))
			{
				selectedEntity = { NullEntityId, nullptr };
				for (auto& callback : onEntitySelectionCallbacks)
					callback(selectedEntity);
			}

			Application::Get()->GetSceneManager()->GetActiveScene()->DestroyEntity(entity);
		}

		ImGui::PopStyleVar();
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
